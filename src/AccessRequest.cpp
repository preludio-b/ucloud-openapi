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

#include "util.h"
#include "AccessRequest.h"

AccessRequest::AccessRequest(void)
{
	m_oauth_callback = OAUTH_CALLBACK;
	m_oauth_signature_method = OAUTH_SIGNATURE_METHOD;
	m_oauth_version = OAUTH_VERSION;
}

AccessRequest::~AccessRequest(void)
{
}

bool AccessRequest::SendAccessRequest()
{
	bool bret;
	
	m_oauth_nonce = get_nonce();
	m_oauth_timestamp = get_timestamp();

	string oheader = makeAuthHeader(hash_hmac(makeBaseString(), m_oauth_consumer_skey+"&"+m_oauth_token_secret));

	SetHeader("Authorization", oheader);

	bret = SendRequest(HTTP_POST);

	//request success
	if(bret) {
        if(HTTP_OK == GetStatusCode()) {
			m_xauth_token = get_split_value(GetResBody(), "oauth_token");
			m_xauth_token_secret = get_split_value(GetResBody(), "oauth_token_secret");
            if(m_xauth_token.size() == 0 || m_xauth_token_secret.size() == 0) {
                m_res_code = get_split_value(GetResBody(), "result_code");
                m_res_msg = get_split_value(GetResBody(), "result_msg");
            }
		}
		else {
			m_res_code = get_split_value(GetResBody(), "result_code");
			m_res_msg = get_split_value(GetResBody(), "result_msg");
		}
	}

	return bret;
}

string AccessRequest::makeBaseString()
{
	string req_param;
    
    SetBaseStringParameter(OAUTH_CALLBACK_STRING, m_oauth_callback);
    SetBaseStringParameter(OAUTH_CONSUMER_KEY_STRING, m_oauth_consumer_key);
    SetBaseStringParameter(OAUTH_NONCE_STRING, m_oauth_nonce);
    SetBaseStringParameter(OAUTH_SIGNATURE_METHOD_STRING, OAUTH_SIGNATURE_METHOD);
    SetBaseStringParameter(OAUTH_TIMESTAMP_STRING, m_oauth_timestamp);
    SetBaseStringParameter(OAUTH_TOKEN_STRING, m_oauth_token);
    SetBaseStringParameter(OAUTH_VERIFIER_STRING, "");
    SetBaseStringParameter(OAUTH_VERSION_STRING, OAUTH_VERSION);
    SetBaseStringParameter(XAUTH_MODE_STRING, XAUTH_MODE);
    SetBaseStringParameter(XAUTH_SERVICETYPE_STIRNG, m_xauth_servicetype);
    SetBaseStringParameter(XAUTH_PASSWORD_STRING, m_xauth_password);
    SetBaseStringParameter(XAUTH_USERNAME_STRING, m_xauth_username);
    
    req_param = urlencode(GetBaseStringParameter());

	string base_string = "POST";
	base_string += "&";
	base_string += urlencode(m_oauth_uri);
	base_string += "&";
	base_string += req_param;

	return base_string;
}

string AccessRequest::makeAuthHeader(string signature)
{
	char buf[1024];

#ifdef WIN32
	sprintf_s(buf, sizeof(buf), OAUTH_AUTH_ACCESS_HEADER,
		m_oauth_callback.c_str(),
		m_oauth_consumer_key.c_str(),
		m_oauth_nonce.c_str(),
		weburlencode(signature).c_str(),
		m_oauth_signature_method.c_str(),
		m_oauth_timestamp.c_str(),
		m_oauth_token.c_str(),
		m_oauth_version.c_str(),
        m_xauth_servicetype.c_str(),
		weburlencode(m_xauth_password).c_str(),
		m_xauth_username.c_str() );
#else
	sprintf(buf, OAUTH_AUTH_ACCESS_HEADER,
		m_oauth_callback.c_str(),
		m_oauth_consumer_key.c_str(),
		m_oauth_nonce.c_str(),
		weburlencode(signature).c_str(),
		m_oauth_signature_method.c_str(),
		m_oauth_timestamp.c_str(),
		m_oauth_token.c_str(),
		m_oauth_version.c_str(),
        m_xauth_servicetype.c_str(),
		weburlencode(m_xauth_password).c_str(),
		m_xauth_username.c_str() );
#endif
	return string(buf);
}
