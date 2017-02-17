/*
 MIT License
 Copyright (c) 2015 kt Inc.
 
 이 소프트웨어의 복제본과 관련된 문서화 파일(“소프트웨어”)을 획득하는 사람은 누구라도
 소프트웨어를 별다른 제한없이 무상으로 사용할 수 있는 권한을 부여 받는다.
 여기에는 소프트웨어의 복제본을 무제한으로 사용, 복제, 수정, 병합, 공표, 배포,
 서브라이선스 설정 및 판매할 수 있는 권리와 이상의 행위를 소프트웨어를 제공받은 다른
 수취인들에게 허용할 수 있는 권리가 포함되며,
 다음과 같은 조건을 충족시키는 것을 전제로 한다.
 
 위와 같은 저작권 안내 문구와 본 허용 문구가 소프트웨어의 모든 복제본 및 중요 부분에 포함되어야 한다.
 
 이 소프트웨어는 상품성, 특정 목적 적합성, 그리고 비침해에 대한 보증을 포함한 어떠한
 형태의 보증도 명시적이나 묵시적으로 설정되지 않은 “있는 그대로의” 상태로 제공된다.
 소프트웨어를 개발한 프로그래머나 저작권자는 어떠한 경우에도 소프트웨어나 소프트웨어의
 사용 등의 행위와 관련하여일어나는 어떤 요구사항이나 손해 및 기타 책임에 대해 계약상,
 불법행위 또는 기타 이유로 인한 책임을 지지 않는다.
 
 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:
 
 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.
 */

#ifdef __APPLE__
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include "util.h"

/*------ Base64 Encoding Table ------*/
static const char MimeBase64[] = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
	'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
	'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
	'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
	'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
	'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
	'w', 'x', 'y', 'z', '0', '1', '2', '3',
	'4', '5', '6', '7', '8', '9', '+', '/'
};

/*------ Base64 Decoding Table ------*/
static size_t DecodeMimeBase64[256] = {
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 00-0F */
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 10-1F */
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,-1,-1,63,  /* 20-2F */
	52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-1,-1,-1,  /* 30-3F */
	-1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,  /* 40-4F */
	15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,  /* 50-5F */
	-1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,  /* 60-6F */
	41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1,  /* 70-7F */
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 80-8F */
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 90-9F */
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* A0-AF */
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* B0-BF */
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* C0-CF */
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* D0-DF */
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* E0-EF */
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1   /* F0-FF */
};

// RFC 3986
string urlencode(const char *inStr)
{
	int inStrLen;
	unsigned short asc;
	BYTE c;
	int i=0;

	string outStr;

	inStrLen = (int)strlen(inStr);
	for(i=0; i<inStrLen; i++)
	{
		c = inStr[i];
		asc = c;
        if( (asc>47 && asc<58) || (asc>64 && asc<91) || (asc>96 && asc<123) || asc == '_' || asc == '.' || asc == '-' || asc == '~' )
			outStr += c;
		else 
		{
			char temp[4]={0,};
#ifdef WIN32
			sprintf_s(temp, sizeof(temp), "%%%2X", asc);
#else
			sprintf(temp, "%%%2X" ,asc);
#endif
			outStr += temp;
		}
	}
	return outStr;
}

string urlencode(string str)
{
	return urlencode(str.c_str());
}

string weburlencode(const char *inStr)
{
    int inStrLen;
    unsigned short asc;
    BYTE c;
    int i=0;
    
    string outStr;
    
    inStrLen = (int)strlen(inStr);
    for(i=0; i<inStrLen; i++)
    {
        c = inStr[i];
        asc = c;
        if( (asc>47 && asc<58) || (asc>64 && asc<91) || (asc>96 && asc<123) )
            outStr += c;
        else
        {
            char temp[4]={0,};
#ifdef WIN32
            sprintf_s(temp, sizeof(temp), "%%%2X", asc);
#else
            sprintf(temp, "%%%2X" ,asc);
#endif
            outStr += temp;
        }
    }
    return outStr;
}

string weburlencode(string str)
{
    return weburlencode(str.c_str());
}

char _x2c(char hex_up, char hex_low)
{
	char digit;

	digit = 16 * (hex_up >= 'A'
		? ((hex_up & 0xdf) - 'A') + 10 : (hex_up - '0'));
	digit += (hex_low >= 'A'
		? ((hex_low & 0xdf) - 'A') + 10 : (hex_low - '0'));
	return (digit);
}

string urldecode(const char * str)
{
	int i = 0;
	string ret;

	if(!str) return ret;

	while(str[i])
	{
		switch(str[i])
		{
		case '+':
			ret += ' ';
			i++;
			break;
		case '%':
			ret += _x2c(str[i+1], str[i+2]);
			i += 3;
			break;
		default:
			ret += str[i];
			i++;
			break;
		}
	}
	return ret;
}
string urldecode(string str) {return urldecode(str.c_str());}

size_t base64_decode(BYTE *dst, const BYTE *text, size_t numBytes)
{
	const BYTE* cp;
	size_t space_idx = 0, phase;
	size_t d, prev_d = 0;
	BYTE c;

	space_idx = 0;
	phase = 0;

	for ( cp = text; *cp != '\0'; ++cp ) {
		d = DecodeMimeBase64[(size_t) *cp];
		if ( d != -1 ) {
			switch ( phase ) {
				case 0:
					++phase;
					break;
				case 1:
					c = ( ( prev_d << 2 ) | ( ( d & 0x30 ) >> 4 ) );
					if ( space_idx < numBytes )
						dst[space_idx++] = c;
					++phase;
					break;
				case 2:
					c = ( ( ( prev_d & 0xf ) << 4 ) | ( ( d & 0x3c ) >> 2 ) );
					if ( space_idx < numBytes )
						dst[space_idx++] = c;
					++phase;
					break;
				case 3:
					c = ( ( ( prev_d & 0x03 ) << 6 ) | d );
					if ( space_idx < numBytes )
						dst[space_idx++] = c;
					phase = 0;
					break;
			}
			prev_d = d;
		}
	}
	return space_idx;
}

size_t base64_encode_raw(const BYTE *text, size_t numBytes, char **encodedText)
{
	BYTE input[3]  = {0,0,0};
	BYTE output[4] = {0,0,0,0};
	size_t   index, i, j, size;
	const BYTE *p, *plen;

	plen           = text + numBytes - 1;
	size           = (4 * (numBytes / 3)) + (numBytes % 3? 4 : 0) + 1;
	(*encodedText) = (char *)malloc(size);
	j              = 0;

	for  (i = 0, p = text;p <= plen; i++, p++) {
		index = i % 3;
		input[index] = *p;

		if (index == 2 || p == plen) {
			output[0] = ((input[0] & 0xFC) >> 2);
			output[1] = ((input[0] & 0x3) << 4) | ((input[1] & 0xF0) >> 4);
			output[2] = ((input[1] & 0xF) << 2) | ((input[2] & 0xC0) >> 6);
			output[3] = (input[2] & 0x3F);

			(*encodedText)[j++] = MimeBase64[output[0]];
			(*encodedText)[j++] = MimeBase64[output[1]];
			(*encodedText)[j++] = index == 0? '=' : MimeBase64[output[2]];
			(*encodedText)[j++] = index <  2? '=' : MimeBase64[output[3]];

			input[0] = input[1] = input[2] = 0;
		}
	}

	(*encodedText)[j] = '\0';

	return 0;
}

string base64_encode(const BYTE * bytes_to_encode, size_t in_len)
{
	char * szEncode = NULL;

	base64_encode_raw( bytes_to_encode, in_len, &szEncode );
	if(!szEncode) return "";

	string ret = szEncode;

	free(szEncode);

	return ret;
}

string base64_encode( string str )
{
	return base64_encode( (const BYTE *)str.c_str(), str.length() );
}

string hash_hmac(string data, string key)
{
    BYTE* result;
    size_t len = 20;
 
    result = (BYTE*)malloc(sizeof(char) * len);
 
    HMAC_CTX ctx;
    HMAC_CTX_init(&ctx);
 
    HMAC_Init_ex(&ctx, key.c_str(), (int)key.length(), EVP_sha1(), NULL);
    HMAC_Update(&ctx, (BYTE*)(data.c_str()), data.length());
    HMAC_Final(&ctx, result, (unsigned int *)(&len));
    HMAC_CTX_cleanup(&ctx);

	string out = base64_encode(result,len);

    free(result);

	return out; 
}

string hash_md5(string data)
{
	unsigned char digest[MD5_DIGEST_LENGTH];
	char buf[10];
	string ret;

	MD5((unsigned char*)data.c_str(), data.size(), (unsigned char*)&digest);

	for(int i = 0 ; i < MD5_DIGEST_LENGTH ; i++)
	{
	#ifdef WIN32
		sprintf_s(buf, sizeof(buf), "%02x", (unsigned int)digest[i]);
	#else
        sprintf(buf, "%02x", (unsigned int)digest[i]);
	#endif
		ret += buf;
	}
	return ret;
}

string get_timestamp()
{
	char buf[1024];
	time_t now;

	time(&now);
#ifdef WIN32
	sprintf_s(buf, sizeof(buf),"%d", now);
#else
    sprintf(buf, "%ld", now);
#endif
	return string(buf);
}

string get_nonce()
{
#ifdef WIN32
    UUID id;
	size_t i;
	char szIID[129];
	wchar_t wszIID[129];

	UuidCreate(&id);
	StringFromGUID2(id, wszIID, 128);
	wcstombs_s(&i, szIID, 128, wszIID, 128);

	return hash_md5(string(szIID));
#elif __APPLE__
    uuid_t uuid;
    string str;
    char buf[3];
    
    uuid_generate(uuid);
    
    for(size_t i = 0 ; i < sizeof(uuid_t) ; i++) {
        sprintf(buf, "%02x", (unsigned int)uuid[i]);
        str += buf;
    }
    return hash_md5(str);
#else
    char buf[1024];
    struct timeval time;
    gettimeofday(&time, NULL);
    long microsec = ((unsigned long long)time.tv_sec * 1000000) + time.tv_usec;

    srand(microsec);
    
    sprintf(buf, "%x%x-%x-%x-%x-%x%x%x",
            rand(), rand(), rand(),
            ((rand() & 0x0fff) | 0x4000),
            rand() % 0x3fff + 0x8000,
            rand(), rand(), rand());
    return hash_md5(buf);
#endif
}

#ifdef WIN32
#define CP_EUCKR 949
string EUCKRtoUTF8(const char * str)
{
	int length;
	string ret;
	wchar_t * uni;
	char * utf8;

	length = MultiByteToWideChar(CP_EUCKR, 0, str, (int)strlen(str)+1, NULL, 0);
	uni = new wchar_t[(length+1)*sizeof(wchar_t)];
	memset(uni, 0x00, (length+1)*sizeof(wchar_t));
	MultiByteToWideChar(CP_EUCKR, 0, str, (int)strlen(str)+1, uni, length);

	length = WideCharToMultiByte(CP_UTF8, 0, uni, -1, NULL, 0, NULL, NULL);
	utf8 = new char[(length+1)*sizeof(char)];
	memset(utf8, 0x00, (length+1)*sizeof(char));
	WideCharToMultiByte(CP_UTF8, 0, uni, -1, utf8, length+1, NULL, NULL);

	ret = utf8;

	delete uni;
	delete utf8;

	return ret;
}

string UTF8toEUCKR(const char * str)
{
	int length;
	string ret;
	wchar_t * uni;
	char * euckr;

	length = MultiByteToWideChar(CP_UTF8, 0, str, (int)strlen(str)+1, NULL, 0);
	uni = new wchar_t[(length+1)*sizeof(wchar_t)];
	memset(uni, 0x00, (length+1)*sizeof(wchar_t));
	MultiByteToWideChar(CP_UTF8, 0, str, (int)strlen(str)+1, uni, length);

	length = WideCharToMultiByte(CP_EUCKR, 0, uni, -1, NULL, 0, NULL, NULL);
	euckr = new char[(length+1)*sizeof(char)];
	memset(euckr, 0x00, (length+1)*sizeof(char));
	WideCharToMultiByte(CP_EUCKR, 0, uni, -1, euckr, length+1, NULL, NULL);

	ret = euckr;

	delete uni;
	delete euckr;

	return ret;
}
#else
string EUCKRtoUTF8(const char * str)
{
    return string(str);
}

string UTF8toEUCKR(const char * str)
{
    return string(str);
}
#endif
string EUCKRtoUTF8(string str) {return EUCKRtoUTF8((const char *)str.c_str());}
string UTF8toEUCKR(string str) {return UTF8toEUCKR((const char *)str.c_str());}

vector<string> split(string str, string separators)
{
	size_t n = str.length();
	size_t start, stop;
	vector<string> result;
	start = str.find_first_not_of(separators);

    while ((start != string::npos) && (start < n)) {
		stop = str.find_first_of(separators, start);
        if ((stop == string::npos) || (stop > n)) stop = n;
		result.push_back(str.substr(start, stop - start));
		start = str.find_first_not_of(separators, stop+1);
	}
	return result;
}

string get_split_value(string data, string key)
{
	vector<string> items = split(data, string("&"));
	vector<string>::iterator it;

	for(it = items.begin() ; it != items.end() ; ++it)
	{
		vector<string> item = split(*it, string("="));
		if(item.size() != 2) continue;
		if( strcmp(key.c_str(), item.at(0).c_str()) == 0 ) return item.at(1);
	}
	return string("");
}

map<string, string> parse(string str)
{
	map<string, string> ret;
	string name;
	string value;

	string tmp = str;
	size_t pos;

	while( ( pos = tmp.find(":") ) != string::npos )
	{
		tmp = tmp.substr(pos+1);
		if( ( pos = tmp.find("=>") ) == string::npos ) break;
		name = tmp.substr(0,pos);
		tmp = tmp.substr(pos+2);
        
        if( tmp.at(0) == '"' ) {
            tmp = tmp.substr(1);
            if( ( pos = tmp.find("\"") ) == string::npos ) break;
            value = tmp.substr(0,pos);
            tmp = tmp.substr(pos+1);
        }
        else {
            if( ( pos = tmp.find(",") ) == string::npos ) {
                if( ( pos = tmp.find("}") ) == string::npos ) break;
            }
            value = tmp.substr(0,pos);
            tmp = tmp.substr(pos+1);
        }

		ret.insert(pair<string,string>(name,value));
	}

	return ret;
}

bool get_file_data(string fname, ByteArray *pData)
{
	struct stat file_info;
	FILE * fp = fopen(fname.c_str(),"rb");
	BYTE * data;
	size_t read;
	size_t bufsize = 1024*1024;

	if(fp == NULL) return false;
#ifdef WIN32
	if(fstat(_fileno(fp), &file_info) != 0)
#else
	if(fstat(fileno(fp), &file_info) != 0)
#endif
	{
		fclose(fp);
		return false;
	}

	data = (BYTE *)malloc(bufsize);
	if( data == NULL ) { fclose(fp); return false;}
	while(true) {
		read = fread(data, sizeof(BYTE), bufsize, fp);
		if(read<=0) break;
		pData->push_back(data, read);
	}
	fclose(fp);

	return true;
}

string get_filename(string fullpathname)
{
    string ret = fullpathname;
    size_t pos;
    
    while( ( pos = ret.find(FOLDER_DELEMETER) ) != string::npos ) ret = ret.substr(pos+1);
    
    return ret;
}

string get_fileext(string filename)
{
    string ret = filename;
    size_t pos;
    
    if( ret.find(".") == string::npos ) return "";
    while( ( pos = ret.find(".") ) != string::npos ) ret = ret.substr(pos+1);
    
    return ret;
}

string get_mediatype(string fileext)
{
    transform( fileext.begin(), fileext.end(), fileext.begin(), ::tolower);
    
    if(fileext.compare("323") == 0) return "text/h323";
    if(fileext.compare("acx") == 0) return "application/internet-property-stream";
    if(fileext.compare("ai") == 0) return "application/postscript";
    if(fileext.compare("aif") == 0) return "audio/x-aiff";
    if(fileext.compare("aifc") == 0) return "audio/aiff";
    if(fileext.compare("aiff") == 0) return "audio/aiff";
    if(fileext.compare("asf") == 0) return "video/x-ms-asf";
    if(fileext.compare("asr") == 0) return "video/x-ms-asf";
    if(fileext.compare("asx") == 0) return "video/x-ms-asf";
    if(fileext.compare("au") == 0) return "audio/basic";
    if(fileext.compare("avi") == 0) return "video/x-msvideo";
    if(fileext.compare("axs") == 0) return "application/olescript";
    if(fileext.compare("bas") == 0) return "text/plain";
    if(fileext.compare("bcpio") == 0) return "application/x-bcpio";
    if(fileext.compare("bin") == 0) return "application/octet-stream";
    if(fileext.compare("bmp") == 0) return "image/bmp";
    if(fileext.compare("c") == 0) return "text/plain";
    if(fileext.compare("cat") == 0) return "application/vndms-pkiseccat";
    if(fileext.compare("cdf") == 0) return "application/x-cdf";
    if(fileext.compare("cer") == 0) return "application/x-x509-ca-cert";
    if(fileext.compare("clp") == 0) return "application/x-msclip";
    if(fileext.compare("cmx") == 0) return "image/x-cmx";
    if(fileext.compare("cod") == 0) return "image/cis-cod";
    if(fileext.compare("cpio") == 0) return "application/x-cpio";
    if(fileext.compare("crd") == 0) return "application/x-mscardfile";
    if(fileext.compare("crl") == 0) return "application/pkix-crl";
    if(fileext.compare("crt") == 0) return "application/x-x509-ca-cert";
    if(fileext.compare("csh") == 0) return "application/x-csh";
    if(fileext.compare("css") == 0) return "text/css";
    if(fileext.compare("dcr") == 0) return "application/x-director";
    if(fileext.compare("der") == 0) return "application/x-x509-ca-cert";
    if(fileext.compare("dib") == 0) return "image/bmp";
    if(fileext.compare("dir") == 0) return "application/x-director";
    if(fileext.compare("dll") == 0) return "application/x-msdownload";
    if(fileext.compare("doc") == 0) return "application/msword";
    if(fileext.compare("docx") == 0) return "application/msword";
    if(fileext.compare("dot") == 0) return "application/msword";
    if(fileext.compare("dvi") == 0) return "application/x-dvi";
    if(fileext.compare("dxr") == 0) return "application/x-director";
    if(fileext.compare("eml") == 0) return "message/rfc822";
    if(fileext.compare("eps") == 0) return "application/postscript";
    if(fileext.compare("etx") == 0) return "text/x-setext";
    if(fileext.compare("evy") == 0) return "application/envoy";
    if(fileext.compare("exe") == 0) return "application/octet-stream";
    if(fileext.compare("fif") == 0) return "application/fractals";
    if(fileext.compare("flr") == 0) return "x-world/x-vrml";
    if(fileext.compare("gif") == 0) return "image/gif";
    if(fileext.compare("gtar") == 0) return "application/x-gtar";
    if(fileext.compare("gz") == 0) return "application/x-gzip";
    if(fileext.compare("h") == 0) return "text/plain";
    if(fileext.compare("hdf") == 0) return "application/x-hdf";
    if(fileext.compare("hlp") == 0) return "application/winhlp";
    if(fileext.compare("hqx") == 0) return "application/mac-binhex40";
    if(fileext.compare("hta") == 0) return "application/hta";
    if(fileext.compare("htc") == 0) return "text/x-component";
    if(fileext.compare("htm") == 0) return "text/html";
    if(fileext.compare("html") == 0) return "text/html";
    if(fileext.compare("htt") == 0) return "text/webviewhtml";
    if(fileext.compare("ico") == 0) return "image/x-icon";
    if(fileext.compare("ief") == 0) return "image/ief";
    if(fileext.compare("iii") == 0) return "application/x-iphone";
    if(fileext.compare("ins") == 0) return "application/x-internet-signup";
    if(fileext.compare("isp") == 0) return "application/x-internet-signup";
    if(fileext.compare("ivf") == 0) return "video/x-ivf";
    if(fileext.compare("jfif") == 0) return "image/pjpeg";
    if(fileext.compare("jpe") == 0) return "image/jpeg";
    if(fileext.compare("jpeg") == 0) return "image/jpeg";
    if(fileext.compare("jpg") == 0) return "image/jpeg";
    if(fileext.compare("jepg") == 0) return "image/jpeg";
    if(fileext.compare("js") == 0) return "application/x-javascript";
    if(fileext.compare("latex") == 0) return "application/x-latex";
    if(fileext.compare("lsf") == 0) return "video/x-la-asf";
    if(fileext.compare("lsx") == 0) return "video/x-la-asf";
    if(fileext.compare("m13") == 0) return "application/x-msmediaview";
    if(fileext.compare("m14") == 0) return "application/x-msmediaview";
    if(fileext.compare("m1v") == 0) return "video/mpeg";
    if(fileext.compare("m3u") == 0) return "audio/x-mpegurl";
    if(fileext.compare("man") == 0) return "application/x-troff-man";
    if(fileext.compare("mdb") == 0) return "application/x-msaccess";
    if(fileext.compare("me") == 0) return "application/x-troff-me";
    if(fileext.compare("mht") == 0) return "message/rfc822";
    if(fileext.compare("mhtml") == 0) return "message/rfc822";
    if(fileext.compare("mid") == 0) return "audio/mid";
    if(fileext.compare("mny") == 0) return "application/x-msmoney";
    if(fileext.compare("mov") == 0) return "video/quicktime";
    if(fileext.compare("movie") == 0) return "video/x-sgi-movie";
    if(fileext.compare("mp2") == 0) return "video/mpeg";
    if(fileext.compare("mp3") == 0) return "audio/mpeg";
    if(fileext.compare("mpa") == 0) return "video/mpeg";
    if(fileext.compare("mpe") == 0) return "video/mpeg";
    if(fileext.compare("mpeg") == 0) return "video/mpeg";
    if(fileext.compare("mpg") == 0) return "video/mpeg";
    if(fileext.compare("mpp") == 0) return "application/vnd.ms-project";
    if(fileext.compare("mppx") == 0) return "application/vnd.ms-project";
    if(fileext.compare("mpv2") == 0) return "video/mpeg";
    if(fileext.compare("ms") == 0) return "application/x-troff-ms";
    if(fileext.compare("mvb") == 0) return "application/x-msmediaview";
    if(fileext.compare("nc") == 0) return "application/x-netcdf";
    if(fileext.compare("nws") == 0) return "message/rfc822";
    if(fileext.compare("oda") == 0) return "application/oda";
    if(fileext.compare("ods") == 0) return "application/oleobject";
    if(fileext.compare("p10") == 0) return "application/pkcs10";
    if(fileext.compare("p12") == 0) return "application/x-pkcs12";
    if(fileext.compare("p7b") == 0) return "application/x-pkcs7-certificates";
    if(fileext.compare("p7c") == 0) return "application/pkcs7-mime";
    if(fileext.compare("p7m") == 0) return "application/pkcs7-mime";
    if(fileext.compare("p7r") == 0) return "application/x-pkcs7-certreqresp";
    if(fileext.compare("p7s") == 0) return "application/pkcs7-signature";
    if(fileext.compare("pbm") == 0) return "image/x-portable-bitmap";
    if(fileext.compare("pdf") == 0) return "application/pdf";
    if(fileext.compare("pfx") == 0) return "application/x-pkcs12";
    if(fileext.compare("pgm") == 0) return "image/x-portable-graymap";
    if(fileext.compare("pko") == 0) return "application/vndms-pkipko";
    if(fileext.compare("pma") == 0) return "application/x-perfmon";
    if(fileext.compare("pmc") == 0) return "application/x-perfmon";
    if(fileext.compare("pml") == 0) return "application/x-perfmon";
    if(fileext.compare("pmr") == 0) return "application/x-perfmon";
    if(fileext.compare("pmw") == 0) return "application/x-perfmon";
    if(fileext.compare("png") == 0) return "image/png";
    if(fileext.compare("pnm") == 0) return "image/x-portable-anymap";
    if(fileext.compare("pot") == 0) return "application/vnd.ms-powerpoint";
    if(fileext.compare("ppm") == 0) return "image/x-portable-pixmap";
    if(fileext.compare("pps") == 0) return "application/vnd.ms-powerpoint";
    if(fileext.compare("ppt") == 0) return "application/vnd.ms-powerpoint";
    if(fileext.compare("pptx") == 0) return "application/vnd.ms-powerpoint";
    if(fileext.compare("prf") == 0) return "application/pics-rules";
    if(fileext.compare("ps") == 0) return "application/postscript";
    if(fileext.compare("pub") == 0) return "application/x-mspublisher";
    if(fileext.compare("qt") == 0) return "video/quicktime";
    if(fileext.compare("ra") == 0) return "audio/x-pn-realaudio";
    if(fileext.compare("ram") == 0) return "audio/x-pn-realaudio";
    if(fileext.compare("ras") == 0) return "image/x-cmu-raster";
    if(fileext.compare("rgb") == 0) return "image/x-rgb";
    if(fileext.compare("rmi") == 0) return "audio/mid";
    if(fileext.compare("roff") == 0) return "application/x-troff";
    if(fileext.compare("rtf") == 0) return "application/rtf";
    if(fileext.compare("rtx") == 0) return "text/richtext";
    if(fileext.compare("scd") == 0) return "application/x-msschedule";
    if(fileext.compare("sct") == 0) return "text/scriptlet";
    if(fileext.compare("setpay") == 0) return "application/set-payment-initiation";
    if(fileext.compare("setreg") == 0) return "application/set-registration-initiation";
    if(fileext.compare("sh") == 0) return "application/x-sh";
    if(fileext.compare("shar") == 0) return "application/x-shar";
    if(fileext.compare("sit") == 0) return "application/x-stuffit";
    if(fileext.compare("snd") == 0) return "audio/basic";
    if(fileext.compare("spc") == 0) return "application/x-pkcs7-certificates";
    if(fileext.compare("spl") == 0) return "application/futuresplash";
    if(fileext.compare("src") == 0) return "application/x-wais-source";
    if(fileext.compare("sst") == 0) return "application/vndms-pkicertstore";
    if(fileext.compare("stl") == 0) return "application/vndms-pkistl";
    if(fileext.compare("stm") == 0) return "text/html";
    if(fileext.compare("sv4cpio") == 0) return "application/x-sv4cpio";
    if(fileext.compare("sv4crc") == 0) return "application/x-sv4crc";
    if(fileext.compare("t") == 0) return "application/x-troff";
    if(fileext.compare("tar") == 0) return "application/x-tar";
    if(fileext.compare("tcl") == 0) return "application/x-tcl";
    if(fileext.compare("tex") == 0) return "application/x-tex";
    if(fileext.compare("texi") == 0) return "application/x-texinfo";
    if(fileext.compare("texinfo") == 0) return "application/x-texinfo";
    if(fileext.compare("tgz") == 0) return "application/x-compressed";
    if(fileext.compare("tif") == 0) return "image/tiff";
    if(fileext.compare("tiff") == 0) return "image/tiff";
    if(fileext.compare("tr") == 0) return "application/x-troff";
    if(fileext.compare("trm") == 0) return "application/x-msterminal";
    if(fileext.compare("tsv") == 0) return "text/tab-separated-values";
    if(fileext.compare("txt") == 0) return "text/plain";
    if(fileext.compare("uls") == 0) return "text/iuls";
    if(fileext.compare("ustar") == 0) return "application/x-ustar";
    if(fileext.compare("vcf") == 0) return "text/x-vcard";
    if(fileext.compare("wav") == 0) return "audio/wav";
    if(fileext.compare("wcm") == 0) return "application/vnd.ms-works";
    if(fileext.compare("wdb") == 0) return "application/vnd.ms-works";
    if(fileext.compare("wks") == 0) return "application/vnd.ms-works";
    if(fileext.compare("wmf") == 0) return "application/x-msmetafile";
    if(fileext.compare("wps") == 0) return "application/vnd.ms-works";
    if(fileext.compare("wri") == 0) return "application/x-mswrite";
    if(fileext.compare("wrl") == 0) return "x-world/x-vrml";
    if(fileext.compare("wrz") == 0) return "x-world/x-vrml";
    if(fileext.compare("xaf") == 0) return "x-world/x-vrml";
    if(fileext.compare("xbm") == 0) return "image/x-xbitmap";
    if(fileext.compare("xla") == 0) return "application/vnd.ms-excel";
    if(fileext.compare("xlc") == 0) return "application/vnd.ms-excel";
    if(fileext.compare("xlm") == 0) return "application/vnd.ms-excel";
    if(fileext.compare("xls") == 0) return "application/vnd.ms-excel";
    if(fileext.compare("xlsx") == 0) return "application/vnd.ms-excel";
    if(fileext.compare("xlt") == 0) return "application/vnd.ms-excel";
    if(fileext.compare("xlw") == 0) return "application/vnd.ms-excel";
    if(fileext.compare("xml") == 0) return "text/xml";
    if(fileext.compare("xof") == 0) return "x-world/x-vrml";
    if(fileext.compare("xpm") == 0) return "image/x-xpixmap";
    if(fileext.compare("xsl") == 0) return "text/xml";
    if(fileext.compare("xwd") == 0) return "image/x-xwindowdump";
    if(fileext.compare("z") == 0) return "application/x-compress";
    if(fileext.compare("zip") == 0) return "application/x-zip-compressed";
    return "application/octet-stream";
}

void replace( string& text, const string find_token, const string replace_token )
{
    size_t i = 0;
    while ((i = text.find(find_token)) != string::npos)
        text.replace(i, find_token.size(), replace_token);
}
