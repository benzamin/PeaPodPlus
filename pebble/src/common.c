#include "common.h"

static uint32_t s_sequence_number = 0xFFFFFFFE;

AppMessageResult ipod_message_out_get(DictionaryIterator **iter_out) {
    AppMessageResult result = app_message_outbox_begin(iter_out);
    if(result != APP_MSG_OK) return result;
    dict_write_int32(*iter_out, IPOD_SEQUENCE_NUMBER_KEY, ++s_sequence_number);
    if(s_sequence_number == 0xFFFFFFFF) {
        s_sequence_number = 1;
    }
    return APP_MSG_OK;
}

void reset_sequence_number() {
    DictionaryIterator *iter = NULL;
    app_message_outbox_begin(&iter);
    if(!iter) return;
    dict_write_int32(iter, IPOD_SEQUENCE_NUMBER_KEY, 0xFFFFFFFF);
    app_message_outbox_send();
}

//AppContextRef g_app_context;

#define INT_DIGITS 5		/* enough for 64 bit integer */

char *itoa(int i)
{
  /* Room for INT_DIGITS digits, - and '\0' */
  static char buf[INT_DIGITS + 2];
	memset(buf, 0, (INT_DIGITS + 2));
  char *p = buf + INT_DIGITS + 1;	/* points to terminating '\0' */
  if (i >= 0) {
    do {
      *--p = '0' + (i % 10);
      i /= 10;
    } while (i != 0);
    return p;
  }
  else {			/* i < 0 */
    do {
      *--p = '0' - (i % 10);
      i /= 10;
    } while (i != 0);
    *--p = '-';
  }
  return p;
}

void itoa1(int num, char* buffer) {
    const char digits[10] = "0123456789";
    buffer[0] = digits[num % 10];
}

void itoa2(int num, char* buffer) {
    const char digits[10] = "0123456789";
    if(num > 99) {
        buffer[0] = '9';
        buffer[1] = '9';
        return;
    } else if(num > 9) {
        buffer[0] = digits[num / 10];
    } else {
        buffer[0] = '0';
    }
    buffer[1] = digits[num % 10];
}

// Milliseconds since January 1st 2012 in some timezone, discounting leap years.
// There must be a better way to do this...
time_t get_pebble_time() {
    time_t now = time(NULL);
  	struct tm *t = localtime(&now);
    time_t seconds = t->tm_sec;
    seconds += t->tm_min * 60;
    seconds += t->tm_hour * 3600;
    seconds += t->tm_yday * 86400;
    seconds += (t->tm_year - 2012) * 31536000;
    return seconds * 1000;
}

/*void format_lap(time_t lap_time, char* buffer) {
    int hundredths = (lap_time / 100) % 10;
    int seconds = (lap_time / 1000) % 60;
    int minutes = (lap_time / 60000) % 60;
    int hours = lap_time / 3600000;

    itoa2(hours, &buffer[0]);
    buffer[2] = ':';
    itoa2(minutes, &buffer[3]);
    buffer[5] = ':';
    itoa2(seconds, &buffer[6]);
    buffer[8] = '.';
    itoa1(hundredths, &buffer[9]);
}*/