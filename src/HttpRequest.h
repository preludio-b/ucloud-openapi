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

#ifndef __HTTPREQUEST_H_DEFINED__
#define __HTTPREQUEST_H_DEFINED__

#include <iostream>
#include <string>
#include <map>
#include <curl/curl.h>
#include "openapi.h"
#include "ByteArray.h"

#if defined(WIN32)
typedef long long int64_t;
#endif

typedef void (*progress)(int64_t, int64_t);

class HttpRequest
{
public:
	enum REQUEST_TYPE
    {
        HTTP_GET,
        HTTP_POST,
        HTTP_PUT,
        HTTP_DELETE
    };

	HttpRequest(void);
	virtual ~HttpRequest(void);

    void SetHeader(string name, string value);
    void SetUrl(string url) {m_url = url;}
    void SetUploadFile(string file) {m_upload_file = file;}
	void SetDownloadFile(string file) {m_download_file = file;}
    void SetParameter(string key, string value);
    void SetParameter(string key, size_t size);
    void SetBaseStringParameter(string key, string value);
	string GetParameter();
	string GetBaseStringParameter();

    void SetData(string data) {SetData((BYTE *)data.c_str(), data.size());}
	void SetData(BYTE * data, size_t len) {req_data.clear(); req_data.push_back(data,len);}
    
    void SetUploadProgress(progress p) {m_uppr = p;}
    void SetDownloadProgress(progress p) {m_dnpr = p;}

	void IgnoreVerifySSL() {m_bVerifySSL = false;}
	void SetCAInfo(string ca){m_cainfo = ca;}

	bool SendRequest(REQUEST_TYPE nType);

	long GetStatusCode() {return m_statusCode;}
    string GetResHeader(void) {return res_header.get_string();}
	string GetResBody(void) {return res_data.get_string();}
	string GetErrorString(void) {return m_strError;}

protected:
	string m_strError;
	string m_download_file;
	string m_upload_file;

private:
	CURL * m_curl;
	struct curl_slist * m_list;
    REQUEST_TYPE m_nType;

	string m_url;

	map<string, string> m_param_map;
	map<string, string> m_basestring_map;

	bool m_bVerifySSL;
	string m_cainfo;
    long m_statusCode;

	ByteArray req_data;
	ByteArray res_data;
	ByteArray res_header;

	//FILE * m_file;
	FILE * m_downloadfp;
    FILE * m_uploadfp;

    void SetInfo(void);

    bool SendGet(string url);
    bool SendPost(string url, BYTE * data, size_t len);
    bool SendPut(string url, string filename);
    bool SendDelete(string url);

	bool SendRequest(void);

    // callback function
	static size_t writeCallBack(char *buf, size_t size, size_t nmemb, void *p)
    {
        return static_cast<HttpRequest*>(p)->saveRequestContent(buf, size, nmemb);
    }

    size_t saveRequestContent(char *buf, size_t size, size_t nmemb)
    {
		if(m_downloadfp) fwrite(buf, size, nmemb, m_downloadfp);
		else res_data.push_back((BYTE *)buf, size*nmemb);
        return (size*nmemb);
    }

    static size_t writeHeaderCallBack(char *buf, size_t size, size_t nmemb, void *p)
    {
        return static_cast<HttpRequest*>(p)->saveRequestHeader(buf, size, nmemb);
    }

    size_t saveRequestHeader(char *buf, size_t size, size_t nmemb)
    {
        res_header.push_back((BYTE *)buf, size*nmemb);
		return (size*nmemb);
    }
    
    static int xferinfo(void *p, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow)
    {
        return static_cast<HttpRequest*>(p)->transferInfo(dltotal, dlnow, ultotal, ulnow);
    }
    
    static int old_xferinfo(void *p, double dltotal, double dlnow, double ultotal, double ulnow)
    {
        return xferinfo(p,(curl_off_t)dltotal, (curl_off_t)dlnow, (curl_off_t)ultotal, (curl_off_t)ulnow);
    }
    
    int transferInfo(curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow)
    {
        if(m_dnpr) if(dltotal > 0) {
            if( prev_t != dltotal || prev_now != dlnow ) {
                m_dnpr((int64_t)dltotal, (int64_t)dlnow);
                prev_t = dltotal; prev_now = dlnow;
            }
        }
        return 0;
    }
    
    static size_t readCallBack(char *buf, size_t size, size_t nmemb, void *p)
    {
        return static_cast<HttpRequest*>(p)->readRequestContent(buf, size, nmemb);
    }
    
    curl_off_t m_uptotal;
    curl_off_t m_upnow;
    
    progress m_dnpr;
    progress m_uppr;
    
    int64_t prev_t;
    int64_t prev_now;
    
    size_t readRequestContent(char *buf, size_t size, size_t nmemb)
    {
        size_t read;
        read = fread(buf, size, nmemb, m_uploadfp);

        m_upnow += read;
        
        if(m_uppr) if(m_uptotal > 0) {
            if( prev_t != m_uptotal || prev_now != m_upnow ) {
                m_uppr((int64_t)m_uptotal, (int64_t)m_upnow);
                prev_t = m_uptotal; prev_now = m_upnow;
            }
        }
        return read;
    }
};

#endif //__HTTPREQUEST_H_DEFINED__
