#ifndef __UTIL_H__
#define __UTIL_H__

void writePid();
inline unsigned char toHex(const unsigned char & x);
inline unsigned char fromHex(const unsigned char & x);
string URLEncode(const string & sIn);
string URLDecode(const string & sIn);

/**
 * return 0 if generate token successful
 */
int genToken(unsigned int uid, time_t time_offset, char * md5_str_buf);

bool IsTokenValid(uint32_t user_id, const char * token);

#endif
