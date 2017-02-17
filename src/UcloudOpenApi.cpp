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

#include "UcloudOpenApi.h"

UcloudOpenApi::UcloudOpenApi()
{
	curl_global_init(CURL_GLOBAL_ALL);
    m_listallmode = false;
}

UcloudOpenApi::~UcloudOpenApi()
{
}

bool UcloudOpenApi::IsKeysExist()
{
	if(0 == m_apikey.size()) {m_resultmsg = "api key required"; return false;}
	if(0 == m_secretkey.size()) {m_resultmsg = "api secretkey required"; return false;}
	if(0 == m_tokenkey.size()) {m_resultmsg = "login required"; return false;}
	if(0 == m_tokensecretkey.size()) {m_resultmsg = "login required"; return false;}

	return true;
}

bool UcloudOpenApi::XAuthLogin(string username, string password, string service_type)
{
	if(0 == username.size()) {m_resultmsg = "username required"; return false;}
	if(0 == password.size()) {m_resultmsg = "password required"; return false;}

	TokenRequest tokenReq;
	AccessRequest accessReq;

	string url;
	string oauthtoken;
	string oauthtokensecret;

	url = OPENAPI_AUTH_TOKEN_PATH;

	tokenReq.SetTokenUrl(url);
#if defined(WIN32)
	tokenReq.IgnoreVerifySSL();
#endif
	tokenReq.SetCustomerKey(m_apikey);
	tokenReq.SetCustomerSKey(m_secretkey);

	if(tokenReq.SendTokenRequest()) {
#ifdef USE_RAW_LOG
        cout << tokenReq.GetResHeader();
        cout << tokenReq.GetResBody() << "\n";
#endif
		if(HTTP_OK == tokenReq.GetStatusCode()) {
			oauthtoken = tokenReq.GetOAuthToken();
			oauthtokensecret = tokenReq.GetOAuthTokenSecret();
		}
		else {
			m_resultcode = tokenReq.GetResCode();
			m_resultmsg = tokenReq.GetResMsg();
			return false;
		}
	}
	else {
		m_resultmsg = tokenReq.GetErrorString();
		return false;
	}

	url = OPENAPI_AUTH_ACCESS_PATH;

	accessReq.SetTokenUrl(url);
	if(m_cainfo.size() > 0) accessReq.SetCAInfo(m_cainfo);

	accessReq.SetCustomerKey(m_apikey);
	accessReq.SetCustomerSKey(m_secretkey);
	accessReq.SetAuthToken(oauthtoken);
	accessReq.SetAuthSToken(oauthtokensecret);

	accessReq.SetUsername(username);
	accessReq.SetPassword(password);
    accessReq.SetServiceType(service_type);

	if(accessReq.SendAccessRequest()) {
#ifdef USE_RAW_LOG
        cout << accessReq.GetResHeader();
        cout << accessReq.GetResBody() << "\n";
#endif
		if(HTTP_OK == accessReq.GetStatusCode()) {
            m_tokenkey = accessReq.GetXAuthToken();
            m_tokensecretkey = accessReq.GetXAuthTokenSecret();
            if(m_tokenkey.size() == 0 || m_tokensecretkey.size() == 0) {
                m_resultcode = accessReq.GetResCode();
                m_resultmsg = accessReq.GetResMsg();
                return false;
            }
		}
		else {
			m_resultcode = accessReq.GetResCode();
			m_resultmsg = accessReq.GetResMsg();
			return false;
		}
	}
	else {
		m_resultmsg = accessReq.GetErrorString();
		return false;
	}

	return true;
}

void UcloudOpenApi::Clear()
{
    m_resultcode.clear();
    m_resultmsg.clear();
    m_userid.clear();
    m_username.clear();
    m_quota.clear();
    m_userspace.clear();
    
    m_hasmore.clear();
    m_parentfolderid.clear();
    m_folderid.clear();
    m_foldername.clear();
    m_createdate.clear();

    m_size.clear();
    m_modifieddate.clear();
    m_mediatype.clear();
    m_publiclink.clear();
    m_presentonserver.clear();
    
    if(false == m_listallmode) {
        m_folders.clear();
        m_files.clear();
    }
    m_fileid.clear();
    m_filetoken.clear();
    m_transfermode.clear();
    m_redirecturl.clear();
}

void UcloudOpenApi::ParseResponse(string ret)
{
    vector<string> items = split(ret, string("&"));
    vector<string>::iterator it;
    string name;
    string value;
    map<string, string> values;
    map<string, string>::iterator it_map;
    string id;
    
    for(it = items.begin() ; it != items.end() ; ++it)
    {
        vector<string> item = split(*it, string("="));
        if(item.size() != 2) continue;
        name = urldecode(item.at(0));
        value = urldecode(item.at(1));
        
        if     (name.compare("result_code")      == 0) m_resultcode = value;
        else if(name.compare("result_msg")       == 0) m_resultmsg = value;
        else if(name.compare("userId")           == 0) m_userid = value;
        else if(name.compare("userName")         == 0) m_username = value;
        else if(name.compare("quota")            == 0) m_quota = value;
        else if(name.compare("usespace")         == 0) m_userspace = value;
        else if(name.compare("folder_name")      == 0) m_foldername = value;
        else if(name.compare("create_date")      == 0) m_createdate = value;
        else if(name.compare("parent_folder_id") == 0) m_parentfolderid = value;
        else if(name.compare("folder_id")        == 0) m_folderid = value;
        else if(name.compare("hasMore")          == 0) m_hasmore = value;
        else if(name.compare("file_name")        == 0) m_filename = value;
        else if(name.compare("size")             == 0) m_size = value;
        else if(name.compare("modified_date")    == 0) m_modifieddate = value;
        else if(name.compare("mediaType")        == 0) m_mediatype = value;
        else if(name.compare("publicLink")       == 0) m_publiclink = value;
        else if(name.compare("presentOnServer")  == 0) m_presentonserver = value;

        else if(name.compare("file_id")          == 0) m_fileid = value;
        else if(name.compare("file_token")       == 0) m_filetoken = value;
        else if(name.compare("transfer_mode")    == 0) m_transfermode = value;
        else if(name.compare("redirect_url")     == 0) m_redirecturl = value;
        
        else if(name.compare(0,7,"Folders") == 0) {
            values = parse(value);
            it_map = values.find("folder_id");
            if(it_map!=values.end()) {
                string fid = it_map->second;
                m_folders.erase(fid);
                m_folders.insert(pair<string, map<string, string> >(fid,values));
            }
        }
        else if(name.compare(0,5,"Files") == 0 || name.compare(0,11,"RecentFiles") == 0 || name.compare(0,9,"LinkFiles") == 0) {
            values = parse(value);
            it_map = values.find("file_id");
            if(it_map!=values.end()) {
                string fid = it_map->second;
                m_files.erase(fid);
                m_files.insert(pair<string, map<string, string> >(fid,values));
            }
        }
    }
}

string UcloudOpenApi::GetFolderValue(size_t idx, const char * name)
{
    if(m_folders.size() == 0) return "";
    if(idx > m_folders.size() - 1) return "";
    
    map<string, map<string, string> >::iterator it;
    map<string, string>::iterator it_value;
    
    it = m_folders.begin();
    for(size_t i=0;i<idx;i++) ++it;
    
    it_value = it->second.find(name);
    
    if( it_value == it->second.end() ) return "";
    
    return it_value->second;
}

string UcloudOpenApi::GetFileValue(size_t idx, const char * name)
{
    if(m_files.size() == 0) return "";
    if(idx > m_files.size() - 1) return "";
    
    map<string, map<string, string> >::iterator it;
    map<string, string>::iterator it_value;
    
    it = m_files.begin();
    for(size_t i=0;i<idx;i++) ++it;
    
    it_value = it->second.find(name);
    
    if( it_value == it->second.end() ) return "";
    
    return it_value->second;
}

void UcloudOpenApi::ApiInit(BaseRequest & req, string url)
{
    req.SetApiUrl(url);
#ifdef WIN32
    req.IgnoreVerifySSL();
#endif
    req.SetApiKeys(m_apikey, m_secretkey);
    req.SetTokenKeys(m_tokenkey, m_tokensecretkey);
    Clear();
}

bool UcloudOpenApi::ApiCall(BaseRequest & req, HttpRequest::REQUEST_TYPE type, const char * success_code, bool userAuthHeader)
{
    string ret;
    
    if(req.SendBaseRequest(type,userAuthHeader)) {
        ret = req.GetResBody();
#ifdef USE_RAW_LOG
        cout << req.GetResHeader();
        cout << req.GetResBody() << "\n";
#endif
        if(HTTP_OK == req.GetStatusCode()) {
            if(success_code) {
                ParseResponse(ret);
                if(m_resultcode.compare(success_code) != 0) {
					return false;
				}
            }
        }
        else {
            m_resultcode = req.GetResCode();
            m_resultmsg = req.GetResMsg();
            return false;
        }
    }
    else {
        m_resultmsg = req.GetErrorString();
        return false;
    }
    return true;
}

bool UcloudOpenApi::GetUserInfo()
{
    BaseRequest baseReq;
    
    ApiInit(baseReq, OPENAPI_GETUSERINFO_PATH);
    
    return ApiCall(baseReq, HttpRequest::HTTP_GET, OPENAPI_SUCCESS);
}

bool UcloudOpenApi::GetSyncFolder(size_t start, size_t max)
{
    BaseRequest baseReq;
    
    ApiInit(baseReq, OPENAPI_GETSYNCFOLDER_PATH);

    baseReq.SetParameter("start", start);
    baseReq.SetParameter("max", max);

    return ApiCall(baseReq, HttpRequest::HTTP_POST, OPENAPI_SUCCESS);
}

bool UcloudOpenApi::GetShareFolder(size_t start, size_t max)
{
    BaseRequest baseReq;
    
    ApiInit(baseReq, OPENAPI_GETSHAREFOLDER_PATH);
    
    baseReq.SetParameter("start", start);
    baseReq.SetParameter("max", max);
    
    return ApiCall(baseReq, HttpRequest::HTTP_POST, OPENAPI_SUCCESS);
}

bool UcloudOpenApi::GetContents(string folderid, size_t start, size_t max)
{
    BaseRequest baseReq;
    
    ApiInit(baseReq, OPENAPI_GETCONTENTS_PATH);
    
    baseReq.SetParameter("folder_id", folderid);
    baseReq.SetParameter("start", start);
    baseReq.SetParameter("max", max);
    
    return ApiCall(baseReq, HttpRequest::HTTP_POST, OPENAPI_SUCCESS);
}

bool UcloudOpenApi::GetAllContents(string folderid)
{
    size_t cnt = OPENAPI_DEFAULT_MAX_COUNT;
    bool ret;
    
    while(true) {
        ret = GetContents(folderid,0,cnt);
        if(false == ret) return false;
        if( m_folders.size() == cnt ) cnt *= 2;
        else break;
    }
    
    if(m_files.size() != cnt) return true;
    
    m_listallmode = true;
    
    while(true) {
        ret = GetContents(folderid,cnt,OPENAPI_DEFAULT_MAX_COUNT*2);
        if(false == ret) {m_listallmode = false; return false;}
        if(false == IsHasMore()) break;
        cnt += (OPENAPI_DEFAULT_MAX_COUNT*2);
    }
    m_listallmode = false;
    
    return true;
}

bool UcloudOpenApi::GetFolderInfo(string folderid)
{
    BaseRequest baseReq;
    
    ApiInit(baseReq, OPENAPI_GETFOLDERINFO_PATH);
    
    baseReq.SetParameter("folder_id", folderid);

    return ApiCall(baseReq, HttpRequest::HTTP_POST, OPENAPI_SUCCESS);
}

bool UcloudOpenApi::CreateFolder(string folderid, string & createdfolderid, string foldername)
{
    BaseRequest baseReq;
    bool ret;
    
    ApiInit(baseReq, OPENAPI_CREATEFOLDER_PATH);
    
    baseReq.SetParameter("folder_id", folderid);
    baseReq.SetParameter("folder_name", foldername);

    ret = ApiCall(baseReq, HttpRequest::HTTP_POST, OPENAPI_CREATED);
    if(ret) { createdfolderid = m_folderid; }
    return ret;
}

bool UcloudOpenApi::DeleteFolder(string folderid)
{
    BaseRequest baseReq;
    
    ApiInit(baseReq, OPENAPI_DELETEFOLDER_PATH);
    
    baseReq.SetParameter("folder_id", folderid);
    
    return ApiCall(baseReq, HttpRequest::HTTP_POST, OPENAPI_MODIFY_SUCCESS);
}

bool UcloudOpenApi::ModifyFolderInfo(string folderid, string foldername)
{
    BaseRequest baseReq;
    
    ApiInit(baseReq, OPENAPI_MODIFYFOLDERINFO_PATH);
    
    baseReq.SetParameter("folder_id", folderid);
    baseReq.SetParameter("folder_name", foldername);
    
    return ApiCall(baseReq, HttpRequest::HTTP_POST, OPENAPI_MODIFY_SUCCESS);
}

bool UcloudOpenApi::DeleteTrash()
{
    BaseRequest baseReq;
    
    ApiInit(baseReq, OPENAPI_DELETETRASH_PATH);
    
    return ApiCall(baseReq, HttpRequest::HTTP_DELETE, OPENAPI_MODIFY_SUCCESS);
}

bool UcloudOpenApi::GetFileLink(size_t start, size_t max)
{
    BaseRequest baseReq;
    
    ApiInit(baseReq, OPENAPI_GETFILELINK_PATH);
    
    baseReq.SetParameter("start", start);
    baseReq.SetParameter("max", max);
    
    return ApiCall(baseReq, HttpRequest::HTTP_POST, OPENAPI_SUCCESS);
}

bool UcloudOpenApi::GetFileInfo(string fileid)
{
    BaseRequest baseReq;
    
    ApiInit(baseReq, OPENAPI_GETFILEINFO_PATH);
    
    baseReq.SetParameter("file_id", fileid);
    
    return ApiCall(baseReq, HttpRequest::HTTP_POST, OPENAPI_SUCCESS);
}

bool UcloudOpenApi::ModifyFileInfo(string fileid, string filename, string mediatype)
{
    //preprocess filename
    if(filename.size() == 0) {return false;}
    if(mediatype.size() == 0) mediatype = get_mediatype(get_fileext(filename));
    
    BaseRequest baseReq;
    
    ApiInit(baseReq, OPENAPI_MODIFYFILEINFO_PATH);
    
    baseReq.SetParameter("file_id", fileid);
    baseReq.SetParameter("file_name", filename);
    baseReq.SetParameter("mediaType", mediatype);
    
    return ApiCall(baseReq, HttpRequest::HTTP_POST, OPENAPI_SUCCESS);
}

bool UcloudOpenApi::UploadFile(string folderid, string & createdfileid, string localfilename, progress p, string filename, string mediatype)
{
    //preprocess filename
    if(localfilename.size() == 0) {return false;}
    if(filename.size() == 0) filename = get_filename(localfilename);
    if(mediatype.size() == 0) mediatype = get_mediatype(get_fileext(filename));
    
    //create fileid
    BaseRequest baseReq;
    
    ApiInit(baseReq, OPENAPI_CREATEFILE_PATH);
    
    baseReq.SetParameter("folder_id", folderid);
    baseReq.SetParameter("file_name", filename);
    baseReq.SetParameter("mediaType", mediatype);
    
    if(false == ApiCall(baseReq, HttpRequest::HTTP_POST, OPENAPI_CREATED)) return false;
    
    createdfileid = m_fileid;
    
    //create upload token
    BaseRequest tokenReq;
    
    ApiInit(tokenReq, OPENAPI_CREATEFILETOKEN_PATH);
    
    tokenReq.SetParameter("file_id", createdfileid);
    tokenReq.SetParameter("transfer_mode", "UP");
    
    if(false == ApiCall(tokenReq, HttpRequest::HTTP_POST, OPENAPI_SUCCESS)) return false;

	string filetoken = m_filetoken;
    //upload file
    BaseRequest uploadReq;

    ApiInit(uploadReq, m_redirecturl);
    
    uploadReq.SetParameter("file_token", filetoken);
    uploadReq.SetUploadFile(localfilename);
    uploadReq.SetUploadProgress(p);
   
    return ApiCall(uploadReq, HttpRequest::HTTP_PUT, NULL, false);
}

bool UcloudOpenApi::DeleteFile(string fileid)
{
    BaseRequest baseReq;
    
    ApiInit(baseReq, OPENAPI_DELETEFILE_PATH);
    
    baseReq.SetParameter("file_id", fileid);
    
    return ApiCall(baseReq, HttpRequest::HTTP_POST, OPENAPI_SUCCESS);
}

bool UcloudOpenApi::DownloadFile(string fileid, string localfilename, progress p)
{
    // get download token
    BaseRequest tokenReq;
    bool ret;
    
    ApiInit(tokenReq, OPENAPI_CREATEFILETOKEN_PATH);
    
    tokenReq.SetParameter("file_id", fileid);
    tokenReq.SetParameter("transfer_mode", "DN");
    
    if(false == ApiCall(tokenReq, HttpRequest::HTTP_POST, OPENAPI_SUCCESS)) return false;
    
	string filetoken = m_filetoken;

	// download file
    BaseRequest baseReq;

    ApiInit(baseReq, m_redirecturl);
    
    baseReq.SetParameter("file_token", filetoken);
    baseReq.SetDownloadFile(localfilename);
    baseReq.SetDownloadProgress(p);
    
    ret = ApiCall(baseReq, HttpRequest::HTTP_GET, NULL, false);
    
    return ret;
}

