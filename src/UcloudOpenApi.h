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

#ifndef __UCLOUDOPENAPI_H_DEFINED__
#define __UCLOUDOPENAPI_H_DEFINED__

#include "openapi.h"
#include "util.h"
#include "HttpRequest.h"
#include "TokenRequest.h"
#include "AccessRequest.h"
#include "BaseRequest.h"

#if defined(WIN32)
typedef long long int64_t;
#endif

typedef void (*progress)(int64_t, int64_t);

class UcloudOpenApi
{
public:
	UcloudOpenApi();
	~UcloudOpenApi();

	//api key setting
    void SetApiKeys(string apikey, string secretkey) {m_apikey = apikey; m_secretkey = secretkey;}
	void SetTokenKeys(string tokenkey, string tokensecretkey) {m_tokenkey = tokenkey; m_tokensecretkey = tokensecretkey;}

	//do login
    bool XAuthLogin(string username, string password, string service_type = "");
    string GetTokenKey() {return m_tokenkey;}
    string GetTokenSecretKey() {return m_tokensecretkey;}
	
    //api call
    bool GetUserInfo();
	bool GetContents(string folderid, size_t start = 0, size_t max = OPENAPI_DEFAULT_MAX_COUNT);
	bool GetAllContents(string folderid);
	bool GetSyncFolder(size_t start = 0, size_t max = OPENAPI_DEFAULT_MAX_COUNT);
	bool GetShareFolder(size_t start = 0, size_t max = OPENAPI_DEFAULT_MAX_COUNT);
	bool GetFolderInfo(string folderid);
	bool CreateFolder(string folderid, string & createdfolderid, string foldername);
	bool DeleteFolder(string folderid);
	bool ModifyFolderInfo(string folderid, string foldername);
    bool DeleteTrash();
	bool GetFileLink(size_t start = 0, size_t max = OPENAPI_DEFAULT_MAX_COUNT);
	bool GetFileInfo(string fileid);
    bool ModifyFileInfo(string fileid, string filename, string mediatype = "");
	bool DeleteFile(string fileid);
	bool UploadFile(string folderid, string & createdfileid, string localfilename, progress p = NULL, string filename = "", string mediatype = "");
	bool DownloadFile(string fileid, string localfilename, progress p = NULL);

	//api call result
    string GetResultCode() {return m_resultcode;}
	string GetResultMsg() {return m_resultmsg;}

	//data query
    size_t GetFoldersCount() {return m_folders.size();}
    size_t GetFilesCount() {return m_files.size();}

    string GetUserId() {return m_userid;}
    string GetUserName() {return m_username;}
    string GetQuita() {return m_quota;}
    string GetUserSpace() {return m_userspace;}
    
    string GetFoldersId(size_t idx) {return GetFolderValue(idx,"folder_id");}
    string GetFoldersName(size_t idx) {return GetFolderValue(idx,"folder_name");}
	string GetFoldersType(size_t idx) {return GetFolderValue(idx,"folder_type");}

    string GetFilesId(size_t idx) {return GetFileValue(idx,"file_id");}
    string GetFilesName(size_t idx) {return GetFileValue(idx,"file_name");}
    string GetFilesSize(size_t idx) {return GetFileValue(idx,"file_size");}
	string GetFilesModifyDate(size_t idx) {return GetFileValue(idx,"modify_date");}
	string GetFilesContentType(size_t idx) {return GetFileValue(idx,"content_type");}
	string GetFilesLastActionType(size_t idx) {return GetFileValue(idx,"action_type");}
    bool IsFilesExist(size_t idx) {return GetFileValue(idx,"presentOnServer").compare("true") == 0 ? true:false;}
    
    bool IsHasMore() {return m_hasmore.compare("Y") == 0 ? true:false;}
    bool IsFileExist() {return m_presentonserver.compare("true") == 0 ? true:false;}
    string GetFolderName() {return m_foldername;}
    string GetFileName() {return m_filename;}
    string GetFileSize() {return m_size;}
    string GetFileModifyDate() {return m_modifieddate;}
    string GetCreateDate() {return m_createdate;}
    string GetFileMediaType() {return m_mediatype;}
    bool IsPublicLink() {return m_publiclink.compare("Y") == 0 ? true:false;}

    string GetParentFolderId() {return m_parentfolderid;}

	// use other ca cert file - for windows curl problem.
	// http://curl.haxx.se/docs/caextract.html
	void SetCAInfo(string ca) {m_cainfo = ca;}
 
private:
	string m_apikey;
	string m_secretkey;
	string m_tokenkey;
	string m_tokensecretkey;

	// data
    string m_resultcode;
	string m_resultmsg;
    string m_userid;
    string m_username;
    string m_quota;
    string m_userspace;
    
    string m_hasmore;
    string m_parentfolderid;
    string m_folderid;
    string m_foldername;
    string m_createdate;
    string m_filename;
    string m_size;
    string m_modifieddate;
    string m_mediatype;
    string m_publiclink;
    string m_presentonserver;
    
    string m_fileid;
    string m_filetoken;
    string m_transfermode;
    string m_redirecturl;

	//cainfo
	string m_cainfo;
    
    map<string, map<string, string> > m_folders;
    map<string, map<string, string> > m_files;
    
    bool m_listallmode;

	bool IsKeysExist();
    
    void Clear();
    void ParseResponse(string ret);
    
    string GetFolderValue(size_t idx, const char * name);
    string GetFileValue(size_t idx, const char * name);
    
    void ApiInit(BaseRequest & req, string url);
    bool ApiCall(BaseRequest & req, HttpRequest::REQUEST_TYPE type, const char * success_code = NULL, bool useAuthHeader = true);
};

#endif // __UCLOUDOPENAPI_H_DEFINED__