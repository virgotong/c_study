#include "fwctrl.h"
#include "rateview.h"
#include "../src_librdkafka/rdkafka.h"
#include "../src_avro/avro.h"

static void dr_msg_cb( rd_kafka_t *rk, const rd_kafka_message_t *rkmessage, void *opaque)
{
	if( rkmessage->err )
	{
		SYSLOG( LOG_ERR, "kafka_callback_error: %s", rd_kafka_err2str(rkmessage->err));
	}
	else
	{
		SYSLOG( LOG_INFO, "Message delivered: [bytes: %zd], [partition: %"PRId32"]", rkmessage->len, rkmessage->partition);
	}
}

int init_schema( int type )
{
	if( type == SCHEMA_RATE )
	{				
		avro_schema_t rate_schema;
		check((avro_schema_from_json_literal(RATE_SCHEMA,&rate_schema)));		
	}
	else if( type == SCHEMA_LINK)
	{
		avro_schema_t link_schema;
		check((avro_schema_from_json_literal(LINK_SCHEMA,&link_schema)));
	}
	else if( type == SCHEMA_FBLINK )
	{
		avro_schema_t fblink_schema;
		check((avro_schema_from_json_literal(FBLINK_SCHEMA,&fblink_schema)));
	}
	else
	{
		avro_schema_t capture_schema;
		check((avro_schema_from_json_literal(CAPTURE_SCHEMA,&capture_schema)));
	}	
	return 0;
}


int add_rate_data( host_rate_data *rate_data, size_t len )
{
	int i;
	avro_file_writer_t db;
	const char *dbname = RATE_DATA_FILE;

	init_schema( SCHEMA_RATE );

	//delete database if it exists
	remove( dbname );

	//create a new database
	if( avro_file_writer_create_with_codec( dbname, rate_schema, &db, QUICKSTOP_CODEC, 0))
	{
		SYSLOG( LOG_ERR, "There was an error when create %s, error_message: %s", dbname, avro_strerror());
		exit(EXIT_FAILURE);
	}
	
	avro_datum_t buf = avro_record( rate_schema );

	for( i = 0; i < len; i++)
	{
		check(avro_record_set( buf, "timestamp", rate_data[i].ts));
		check(avro_record_set( buf, "address", tip_addr_ntop(rate_data[i].address)));

		//input 
		check(avro_record_set( buf, "input_bps", BYTE_TO_BPS(rate_data[i].max_hrv.values[ STAT_BPS_INCOMING ])));
		check(avro_record_set( buf, "input_pps", BYTE_TO_BPS(rate_data[i].max_hrv.values[ STAT_PPS_INCOMING ])));
		check(avro_record_set( buf, "input_bps_acc", BYTE_TO_BPS(rate_data[i].nAccumulatedFlow_Input[0])));
		check(avro_record_set( buf, "input_submit_bps", BYTE_TO_BPS(rate_data[i].max_hrv.values[ STAT_BPS_INCOMING_PASS ])));
		check(avro_record_set( buf, "input_submit_pps", rate_data[i].max_hrv.values[ STAT_PPS_INCOMING_PASS ]));
		check(avro_record_set( buf, "input_submit_acc", BYTE_TO_BPS(rate_data[i].nAccumulatedFlow_Submit[0])));

		//output
		check(avro_record_set( buf, "output_bps", BYTE_TO_BPS(rate_data[i].max_hrv.values[ STAT_BPS_OUTGOING ])));
		check(avro_record_set( buf, "output_pps", BYTE_TO_BPS(rate_data[i].max_hrv.values[ STAT_PPS_OUTGOING ])));
		check(avro_record_set( buf, "output_bps_acc", BYTE_TO_BPS(rate_data[i].nAccumulatedFlow_Input[1])));
		check(avro_record_set( buf, "output_submit_bps", BYTE_TO_BPS(rate_data[i].max_hrv.values[ STAT_BPS_OUTGOING_PASS ])));
		check(avro_record_set( buf, "output_submit_pps", rate_data[i].max_hrv.values[ STAT_PPS_OUTGOING_PASS ]));
		check(avro_record_set( buf, "output_submit_acc", BYTE_TO_BPS(rate_data[i].nAccumulatedFlow_Submit[1])));

		//rate
		check(avro_record_set( buf, "syn_rate", rate_data[i].max_hrv.values[ STAT_TCP_SYN ]));
		check(avro_record_set( buf, "ack_rate", rate_data[i].max_hrv.values[ STAT_TCP_ACK_RST ]));
		check(avro_record_set( buf, "udp_rate", rate_data[i].max_hrv.values[ STAT_UDP ]));
		check(avro_record_set( buf, "icmp_rate", rate_data[i].max_hrv.values[ STAT_ICMP ]));
		check(avro_record_set( buf, "frag_rate", rate_data[i].max_hrv.values[ STAT_Fragments ]));
		check(avro_record_set( buf, "nonip_rate", rate_data[i].max_hrv.values[ STAT_NonIP ]));
		check(avro_record_set( buf, "new_tcp_rate", rate_data[i].max_hrv.values[ STAT_NewTCP ]));

		//connections
		check(avro_record_set( buf, "tcp_conn_in", rate_data[i].max_hrv.nConnections_TCP_in));
		check(avro_record_set( buf, "tcp_conn_out",rate_data[i].max_hrv.nConnections_TCP_out));
		check(avro_record_set( buf, "udp_conn", rate_data[i].max_hrv.nConnections_UDP));

		//append buf to db
		check(avro_file_writer_append( db, buf ));

		SYSLOG(LOG_INFO, "Successfully append buf to db!");
	}

	avro_file_writer_flush( db );
	avro_file_writer_close( db );
	avro_schema_decref( rate_schema );

	SYSLOG(LOG_INFO,"add data to db file is finish");
	return 0;
}

int data_to_kafka( int db_type )
{
	rd_kafka_t *rk;
	rd_kafka_topic_t *rkt;
	rd_kafka_conf_t *conf;

	//librdkafka error reporting buffer
	char errstr[512];

	const char *brokers;
	const char *topic;

	brokers = "172.37.37.14";
	topic   = "test";

	//conf set
	conf = rd_kafka_conf_new();
	if(rd_kafka_conf_set( conf, "bootstrap.servers", brokers, errstr, sizeof(errstr) ) != RD_KAFKA_CONF_OK)
	{
		SYSLOG(LOG_ERR,"rdkafka configure error message:%s",errstr);
	}

	//callback set
	rd_kafka_conf_set_dr_msg_cb( conf, dr_msg_cb );

	//produce set
	rk = rd_kafka_new( RD_KAFKA_PRODUCER, conf, errstr, sizeof(errstr) );
	if(!rk)
	{
		SYSLOG(LOG_ERR,"Failed to create new producer : %s",errstr);
	}

	//topic set
	rkt = rd_kafka_topic_new( rk, topic, NULL );
	if(!rkt)
	{
		SYSLOG(LOG_ERR,"Falied to create topic object: %s", errstr);
	}

	//read db file to produce
	FILE *fp;
	
	switch( db_type )
	{
		case SCHEMA_RATE:
			fp = fopen(RATE_DATA_FILE,"r");
			break;

		case SCHEMA_LINK:
			fp = fopen(LINKS_DATA_FILE,"r");
			break;

		case SCHEMA_FBLINK:
			fp = fopen(FBLINKS_DATA_FILE,"r");
			break;

		case SCHEMA_CAPTURE:
			fp = fopen(CAPTURE_DATA_FILE,"r");
			break;

		default:
			SYSLOG(LOG_INFO, "No rate_db_file can read!!!");
			break;
	}

	fseek( fp, 0L, SEEK_END );
	size_t flen = ftell( fp );
	char *p = (char *)malloc(flen + 1);

	if(p == NULL)
	{
		fclose(fp);
	}

	int val;
	fseek( fp, 0L, SEEK_SET );
	val = fread( p, flen, 1, fp );
	fclose(fp);

	if(rd_kafka_produce(rkt, RD_KAFKA_PARTITION_UA, RD_KAFKA_MSG_F_COPY, p, flen, NULL, 0, NULL) == -1)
	{
		SYSLOG(LOG_ERR,"Failed to produce to topic %s: %s", rd_kafka_topic_name(rkt), rd_kafka_err2str(rd_kafka_last_error()));

		if(rd_kafka_last_error() == RD_KAFKA_RESP_ERR__QUEUE_FULL) // QUERY IS FULL
		{
			rd_kafka_poll(rk, 1000);
		}
	}
	else
	{

		SYSLOG(LOG_INFO,"Enqueued message (%d bytes) for topic %s and val is %d", flen, rd_kafka_topic_name(rkt),val);
	}
	rd_kafka_poll(rk,0); //non-blocking
	rd_kafka_flush(rk,10*1000);//10s
	rd_kafka_topic_destroy(rkt);
	rd_kafka_destroy(rk);
	if(p)
	{
		free(p);
	}
}

