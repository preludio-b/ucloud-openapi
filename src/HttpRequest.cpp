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
#include "util.h"
#include "HttpRequest.h"

HttpRequest::HttpRequest(void)
{
	m_list = NULL;
	m_downloadfp = NULL;
    m_uploadfp = NULL;
	m_bVerifySSL = true;
    
    m_uppr = NULL;
    m_dnpr = NULL;
    
    prev_t = 0;
    prev_now = 0;

    m_curl = curl_easy_init();
    curl_easy_setopt(m_curl, CURLOPT_FOLLOWLOCATION, 1L);
}

HttpRequest::~HttpRequest(void)
{
	if(m_curl) curl_easy_cleanup(m_curl);
	if(m_list) curl_slist_free_all(m_list);
}

void HttpRequest::SetHeader(string name, string value)
{
    string strHeader = name;
    strHeader += ":";
    if(value.size() > 0) {
        strHeader += " ";
        strHeader += value;
    }
    m_list = curl_slist_append(m_list, strHeader.c_str());
}

void HttpRequest::SetInfo(void)
{
    if(!m_bVerifySSL) {
        curl_easy_setopt(m_curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(m_curl, CURLOPT_SSL_VERIFYPEER, 0L);
    }
	else {
        curl_easy_setopt(m_curl, CURLOPT_SSL_VERIFYHOST, 1L);
        curl_easy_setopt(m_curl, CURLOPT_SSL_VERIFYPEER, 1L);
	}
	if(m_cainfo.size() > 0) curl_easy_setopt(m_curl, CURLOPT_CAINFO, m_cainfo.c_str());

	res_header.clear();
	res_data.clear();
	
	m_list = curl_slist_append(m_list, "Expect:");

	curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, m_list);

	curl_easy_setopt(m_curl, CURLOPT_WRITEHEADER, this);
    curl_easy_setopt(m_curl, CURLOPT_HEADERFUNCTION, writeHeaderCallBack);
	curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, writeCallBack);
    
    curl_easy_setopt(m_curl, CURLOPT_PROGRESSDATA, this);
    curl_easy_setopt(m_curl, CURLOPT_PROGRESSFUNCTION, old_xferinfo);
#if LIBCURL_VERSION_NUM >= 0x072000
    curl_easy_setopt(m_curl, CURLOPT_XFERINFODATA, this);
    curl_easy_setopt(m_curl, CURLOPT_XFERINFOFUNCTION, xferinfo);
#endif
    curl_easy_setopt(m_curl, CURLOPT_NOPROGRESS, 0L);
}

bool HttpRequest::SendGet(string url)
{
    bool ret;

	SetInfo();

	if(m_download_file.size() > 0) {
		m_downloadfp = fopen(m_download_file.c_str(),"wb");
		if(m_downloadfp == NULL) {
			m_strError = string("can not open file : ") + m_download_file;
			return false;
		}
	}

    curl_easy_setopt(m_curl, CURLOPT_URL, url.c_str());

    ret = SendRequest();

	if(m_downloadfp) {
		if(false == ret ) {
#ifdef WIN32
			_unlink(m_download_file.c_str());
#else
			unlink(m_download_file.c_str());
#endif
		}
		fclose(m_downloadfp);
		m_downloadfp = NULL;
	}

	return ret;
}

bool HttpRequest::SendPost(string url, BYTE * data, size_t len)
{
    SetInfo();

    curl_easy_setopt(m_curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(m_curl, CURLOPT_POSTFIELDSIZE, len);
    curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, data);

    return SendRequest();
}

bool HttpRequest::SendPut(string url, string filename)
{
    bool ret;
	
	SetInfo();

#ifdef WIN32
    struct _stat64 file_info;
#else
    struct stat64 file_info;
#endif
    m_uploadfp = fopen(filename.c_str(),"rb");
    if(m_uploadfp == NULL) {m_strError = filename + " fopen error"; return false;}
#ifdef WIN32
    if(_fstat64(_fileno(m_uploadfp), &file_info) != 0)
#else
    if(fstat64(fileno(m_uploadfp), &file_info) != 0)
#endif
    {
        m_strError = filename + " fstat64 error";
        fclose(m_uploadfp); m_uploadfp = NULL;
        return false;
    }
    m_uptotal = (curl_off_t)file_info.st_size;
    m_upnow = 0;
    
    curl_easy_setopt(m_curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(m_curl, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(m_curl, CURLOPT_READDATA, m_uploadfp);
    curl_easy_setopt(m_curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)file_info.st_size);
    curl_easy_setopt(m_curl, CURLOPT_READDATA, this);
    curl_easy_setopt(m_curl, CURLOPT_READFUNCTION, readCallBack);
    
    ret = SendRequest();
    
    fclose(m_uploadfp); m_uploadfp = NULL;
    return ret;
}

bool HttpRequest::SendDelete(string url)
{
    SetInfo();
    
    curl_easy_setopt(m_curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(m_curl, CURLOPT_CUSTOMREQUEST, "DELETE");
    
    return SendRequest();
}

bool HttpRequest::SendRequest(void)
{
    CURLcode res;

	//show libcurl detail log
#ifdef USE_LIBCURL_LOG
    curl_easy_setopt(m_curl, CURLOPT_VERBOSE, 1L);
#endif
    res = curl_easy_perform(m_curl);
    m_uppr = NULL;
    m_dnpr = NULL;
    if(CURLE_OK != res) {
        m_strError = curl_easy_strerror(res);
        return false;
    }
    curl_easy_getinfo(m_curl, CURLINFO_RESPONSE_CODE, &m_statusCode);
    return true;
}

bool HttpRequest::SendRequest(REQUEST_TYPE nType)
{
	switch( nType ) {
        case HTTP_POST:
#ifdef USE_RAW_LOG
            cout << "POST DATA : " << req_data.get_string() << "\n";
#endif
            return SendPost(m_url, req_data.get_buffer(), req_data.size());
            break;
        case HTTP_PUT:
            return SendPut(m_url,m_upload_file);
            break;
        case HTTP_DELETE:
            return SendDelete(m_url);
            break;
        case HTTP_GET:
        default:
            return SendGet(m_url);
            break;
    }
	return true;
}

void HttpRequest::SetParameter(string key, string value)
{
	m_param_map.erase(key);
	m_param_map.insert(pair<string,string>(key,value));

	SetBaseStringParameter(key, value);
}

void HttpRequest::SetParameter(string key, size_t size)
{
    char buf[1024];
    sprintf(buf, "%lu", size);
    SetParameter(key, string(buf));
}

void HttpRequest::SetBaseStringParameter(string key, string value)
{
	m_basestring_map.erase(key);
	m_basestring_map.insert(pair<string,string>(key,value));
}

string HttpRequest::GetParameter()
{
	string ret;
	map<string,string>::iterator it;
	
	for(it = m_param_map.begin() ; it != m_param_map.end() ; ++it)
	{
		if(ret.size()!= 0) ret += "&";
		ret += urlencode(it->first.c_str());
		ret += "=";
		ret += urlencode(it->second.c_str());
	}
	return ret;
}

string HttpRequest::GetBaseStringParameter()
{
	string ret;
	map<string,string>::iterator it;

	for(it = m_basestring_map.begin() ; it != m_basestring_map.end() ; ++it)
	{
		if(ret.size()!= 0) ret += "&";
		ret += urlencode(it->first.c_str());
		ret += "=";
		ret += urlencode(it->second.c_str());
	}
	return ret;
}
