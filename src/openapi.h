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

#ifndef __OPENAPI_H_DEFINED__
#define __OPENAPI_H_DEFINED__

#ifdef WIN32
#include <windows.h>
#include <rpc.h>
#elif __APPLE__
#include <uuid/uuid.h>
#else
#endif
#include <openssl/hmac.h>
#include <openssl/md5.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifndef WIN32
#include <sys/time.h>
#include <unistd.h>
#endif
#include <time.h>
#include <stdio.h>
#include <string.h>

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <algorithm>

#ifndef BYTE
typedef unsigned char BYTE;
#endif

#ifdef WIN32
#define FOLDER_DELEMETER    "\\"
#else
#define FOLDER_DELEMETER    "/"
#endif

//#define USE_LIBCURL_LOG
//#define USE_RAW_LOG

#define HTTP_OK 200

#define OPENAPI_SUCCESS                 "200"
#define OPENAPI_CREATED                 "201"
#define OPENAPI_MODIFY_SUCCESS          "204"

#define OPENAPI_DEFAULT_RECENTFILE_COUNT 25
#define OPENAPI_DEFAULT_MAX_COUNT       500

#define OPENAPI_AUTH_TOKEN_PATH			"https://openapi.ucloud.com:553/ucloud/oauth/1.0a/request_token"
#define OPENAPI_AUTH_ACCESS_PATH		"https://openapi.ucloud.com:553/ucloud/oauth/1.0a/access_token"

#define OPENAPI_GETUSERINFO_PATH		"https://openapi.ucloud.com:553/ucloud/api/1.0/ucloud/basic/getuserinfo"
#define OPENAPI_GETSYNCFOLDER_PATH		"https://openapi.ucloud.com:553/ucloud/api/1.0/ucloud/basic/getsyncfolder"
#define OPENAPI_GETSHAREFOLDER_PATH		"https://openapi.ucloud.com:553/ucloud/api/1.0/ucloud/basic/getsharefolder"

#define OPENAPI_GETCONTENTS_PATH		"https://openapi.ucloud.com:553/ucloud/api/1.0/ucloud/basic/getcontents"
#define OPENAPI_GETFOLDERINFO_PATH		"https://openapi.ucloud.com:553/ucloud/api/1.0/ucloud/basic/getfolderinfo"
#define OPENAPI_CREATEFOLDER_PATH		"https://openapi.ucloud.com:553/ucloud/api/1.0/ucloud/basic/createfolder"
#define OPENAPI_DELETEFOLDER_PATH		"https://openapi.ucloud.com:553/ucloud/api/1.0/ucloud/basic/deletefolder"
#define OPENAPI_MODIFYFOLDERINFO_PATH	"https://openapi.ucloud.com:553/ucloud/api/1.0/ucloud/basic/modifyfolderinfo"
#define OPENAPI_DELETETRASH_PATH        "https://openapi.ucloud.com:553/ucloud/api/1.0/ucloud/basic/deletetrash"

#define OPENAPI_GETFILELINK_PATH		"https://openapi.ucloud.com:553/ucloud/api/1.0/ucloud/basic/getfilelink"
#define OPENAPI_GETFILEINFO_PATH		"https://openapi.ucloud.com:553/ucloud/api/1.0/ucloud/basic/getfileinfo"
#define OPENAPI_MODIFYFILEINFO_PATH		"https://openapi.ucloud.com:553/ucloud/api/1.0/ucloud/basic/modifyfileinfo"
#define OPENAPI_DELETEFILE_PATH			"https://openapi.ucloud.com:553/ucloud/api/1.0/ucloud/basic/deletefile"
#define OPENAPI_CREATEFILE_PATH			"https://openapi.ucloud.com:553/ucloud/api/1.0/ucloud/basic/createfile"
#define OPENAPI_CREATEFILETOKEN_PATH	"https://openapi.ucloud.com:553/ucloud/api/1.0/ucloud/basic/createfiletoken"

#define OAUTH_CALLBACK_STRING			"oauth_callback"
#define OAUTH_CALLBACK					"http://localhost"
#define OAUTH_CONSUMER_KEY_STRING		"oauth_consumer_key"
#define OAUTH_NONCE_STRING				"oauth_nonce"
#define OAUTH_SIGNATURE_METHOD_STRING	"oauth_signature_method"
#define OAUTH_SIGNATURE_METHOD			"HMAC-SHA1"
#define OAUTH_TIMESTAMP_STRING			"oauth_timestamp"
#define OAUTH_VERIFIER_STRING			"oauth_verifier"
#define OAUTH_VERSION_STRING			"oauth_version"
#define OAUTH_VERSION					"1.0"
#define OAUTH_TOKEN_STRING				"oauth_token"
#define OAUTH_VERIFIER_STRING			"oauth_verifier"

#define XAUTH_MODE_STRING				"x_auth_mode"
#define XAUTH_MODE						"client_auth"
#define XAUTH_USERNAME_STRING			"x_auth_username"
#define XAUTH_PASSWORD_STRING			"x_auth_password"
#define XAUTH_SERVICETYPE_STIRNG        "x_auth_service_type"

#define OAUTH_API_TOKEN_STRING			"api_token"

#define OAUTH_AUTH_TOKEN_HEADER			"OAuth oauth_callback=\"%s\",oauth_consumer_key=\"%s\",oauth_nonce=\"%s\",oauth_signature=\"%s\",oauth_signature_method=\"%s\",oauth_timestamp=\"%s\",oauth_version=\"%s\""
#define OAUTH_AUTH_ACCESS_HEADER		"OAuth oauth_callback=\"%s\",oauth_consumer_key=\"%s\",oauth_nonce=\"%s\",oauth_signature=\"%s\",oauth_signature_method=\"%s\",oauth_timestamp=\"%s\",oauth_token=\"%s\",oauth_verifier=\"\",oauth_version=\"%s\",realm=\"userAuth\",x_auth_mode=\"client_auth\",x_auth_service_type=\"%s\",x_auth_password=\"%s\",x_auth_username=\"%s\""
#define API_REQUEST_HEADER				"OAuth oauth_consumer_key=\"%s\",oauth_nonce=\"%s\",oauth_signature=\"%s\",oauth_signature_method=\"%s\",oauth_timestamp=\"%s\",oauth_token=\"%s\",oauth_version=\"%s\",realm=\"userAuth\""

using namespace std;

#endif//__OPENAPI_H_DEFINED__