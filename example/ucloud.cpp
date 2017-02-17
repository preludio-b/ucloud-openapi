#include "openapi.h"
#include "util.h"
#include "TokenRequest.h"
#include "AccessRequest.h"
#include "BaseRequest.h"
#include "UcloudOpenApi.h"
#include "testkey.h"
#include "../test/ucloud_sdk_test.cpp"

static void download_progress(int64_t total, int64_t now)
{
    cout << "download total = " << total << " now = " << now << "\n";
}

static void upload_progress(int64_t total, int64_t now)
{
    cout << "upload total = " << total << " now = " << now << "\n";
}

bool download_folder( string curpath, string pfid, UcloudOpenApi *papi )
{
	UcloudOpenApi api;
	string fname;
	string fid;
	size_t i;
	size_t r;
	bool b;

	api.SetApiKeys( TEST_APIKEY, TEST_APISECRETKEY );
	api.SetTokenKeys( papi->GetTokenKey(), papi->GetTokenSecretKey() );

	if( api.GetAllContents( pfid ) == false ) {
		printf("%s - %s get info error\n", curpath.c_str(), fid.c_str() );
		return false;
	}

	// 폴더 처리
	for( i = 0 ; i < api.GetFoldersCount() ; i++ ) {
		fname = UTF8toEUCKR(api.GetFoldersName(i));
        fid = api.GetFoldersId(i);

		printf("%s\\%s createfolder\n", curpath.c_str(), fname.c_str() );

		// 폴더 생성
		if( FALSE == CreateDirectory( (curpath+"\\"+fname).c_str(), NULL ) ) {
			if ( ERROR_ALREADY_EXISTS != GetLastError() ) {
				printf("%s\\%s createfolder error\n", curpath.c_str(), fname.c_str() );
				return false;
			}
		}
		// 폴더 조회
		if( false == download_folder( curpath+"\\"+fname, fid, papi ) ) return false;
	}

	// 파일 처리
	for( i = 0 ; i < api.GetFilesCount() ; i++ ) {
		fname = UTF8toEUCKR(api.GetFilesName(i));
        fid = api.GetFilesId(i);

		struct stat buf;
		if( 0 == stat( (curpath+"\\"+fname).c_str(), &buf ) ) {
			if( atoi(api.GetFilesSize(i).c_str()) == buf.st_size ) {
				printf("%s\\%s existed\n", curpath.c_str(), fname.c_str() );	
				continue;
			}
		}
		
		r = 100;
		b = true;

		while (b) {
			if( false == api.DownloadFile( fid, curpath+"\\"+fname ) ) {
				printf("%s\\%s download error\n", curpath.c_str(), fname.c_str() );
				//return false;
				r--;
			}
			else {
				printf("%s\\%s downloaded\n", curpath.c_str(), fname.c_str() );
				b = false;
			}
			if( r == 0 ) b = false;;
			if(!b) printf("%s\\%s download retry\n", curpath.c_str(), fname.c_str() );
		}
	}
	return true;
}

int main(int argc, char ** argv)
{
    //test();
    //return 0;
	string downpath = "d:\\ucloud";
    
    UcloudOpenApi api;

    string apikey = TEST_APIKEY;
    string secretkey = TEST_APISECRETKEY;
    string tokenkey = TEST_TOKENEKY;
    string tokensecretkey = TEST_TOKENSECRETKEY;
    string username = TEST_USERNAME;
    string password = TEST_PASSWORD;
    string servicetype = TEST_SERVICE_TYPE;
    
    string test_fid;
    string test_created_fid;
    string test_created_fileid;

    size_t i;
    
	api.SetApiKeys(apikey, secretkey);
#ifdef WIN32
	// http://curl.haxx.se/docs/caextract.html
	api.SetCAInfo("C:\\curl-7.40.0\\bin\\cacert.pem");
#endif

    // login
    if( tokenkey.size() > 0 && tokensecretkey.size() > 0 ) {
        api.SetTokenKeys(tokenkey, tokensecretkey);
    }
    else if(api.XAuthLogin(username, password, servicetype)) {
		cout << "success:" << api.GetTokenKey() << ":" << api.GetTokenSecretKey() << "\n";
	}
	else {
		cout << "fail:" << api.GetResultCode() << ":" << UTF8toEUCKR(api.GetResultMsg()) << "\n";
        return -1;
	}

	// get user info
    cout << "\nGetUserInfo ----\n";
    if(api.GetUserInfo()) {
        PrintData(api);
	}
	else {
		cout << "fail:" << api.GetResultCode() << ":" << UTF8toEUCKR(api.GetResultMsg()) << "\n";
        return -1;
	}

    // get sync folder info
    cout << "\nGetSyncFolder ----\n";
    if(api.GetSyncFolder()) {
        PrintData(api);
    }
    else {
        cout << "fail:" << api.GetResultCode() << ":" << UTF8toEUCKR(api.GetResultMsg()) << "\n";
        return -1;
    }
    
    // find test folder id -> use cloud folder
    for(i = 0 ; i < api.GetFoldersCount() ; i++)
    {
        if(api.GetFoldersType(i).compare("Cloud") == 0) {
            test_fid = api.GetFoldersId(i);
        }
    }
    if(test_fid.size() == 0) {
        cout << "cloud folder not found!\n";
        return false;
    }
    cout << "\ntest folder id : " << test_fid << "\n";
 
	download_folder( downpath, test_fid, &api );

	return true;

    // get share folder
    cout << "\nGetShareFolder ----\n";
    if(api.GetShareFolder()) {
        PrintData(api);
    }
    else {
        cout << "fail:" << api.GetResultCode() << ":" << UTF8toEUCKR(api.GetResultMsg()) << "\n";
        return -1;
    }

    // get contents list
    cout << "\nGetContents ---- " << test_fid << "\n";
    if(api.GetContents(test_fid)) {
        PrintData(api);
    }
    else {
        cout << "fail:" << api.GetResultCode() << ":" << UTF8toEUCKR(api.GetResultMsg()) << "\n";
        return -1;
    }

    // get folder info
    cout << "\nGetFolderInfo ---- " << test_fid << "\n";
    if(api.GetFolderInfo(test_fid)) {
        PrintData(api);
    }
    else {
        cout << "fail:" << api.GetResultCode() << ":" << UTF8toEUCKR(api.GetResultMsg()) << "\n";
        return -1;
    }

    // create folder
    cout << "\nCreateFolder ----\n";
    if(api.CreateFolder(test_fid, test_created_fid, "U")) {
        cout << "create-id : " << test_created_fid << "\n";
    }
    else {
        cout << "fail:" << api.GetResultCode() << ":" << UTF8toEUCKR(api.GetResultMsg()) << "\n";
        return -1;
    }

    // modify folder name
    cout << "\nModifyFolderInfo ----\n";
    if(api.ModifyFolderInfo(test_created_fid, string("change my folder"))) {
        PrintData(api);
    }
    else {
        cout << "fail:" << api.GetResultCode() << ":" << UTF8toEUCKR(api.GetResultMsg()) << "\n";
        return -1;
    }
    
    // get all contents list
    // if there are lots of folders of files, it takes a long time.
    cout << "\nGetAllContents ---- " << test_fid << "\n";
    if(api.GetAllContents(test_fid)) {
        PrintData(api);
    }
    else {
        cout << "fail:" << api.GetResultCode() << ":" << UTF8toEUCKR(api.GetResultMsg()) << "\n";
        return -1;
    }

    // find created folder id
    for(i = 0 ; i < api.GetFoldersCount() ; i++)
    {
        if(api.GetFoldersId(i).compare(test_created_fid) == 0) {
            cout << "folder created - " << test_created_fid << "\n";
            if( api.GetFoldersName(i).compare("change my folder") == 0 ) {
                cout << "folder renamed - " << "change my folder" << "\n";
            }
        }
    }

    // delete folder
    // you can find foder in trash.
    cout << "\nDeleteFolder ---- " << test_created_fid << "\n";
    if(api.DeleteFolder(test_created_fid)) {
        PrintData(api);
    }
    else {
        cout << "fail:" << api.GetResultCode() << ":" << UTF8toEUCKR(api.GetResultMsg()) << "\n";
        return -1;
    }

    cout << "\nUploadFile ----\n";
	string upfilename = TEST_UPLOADFILE;

    if(api.UploadFile(test_fid, test_created_fileid, upfilename, upload_progress)) {
        PrintData(api);
        cout << test_created_fileid << " : " << upfilename << "\n";
    }
    else {
        cout << "fail:" << api.GetResultCode() << ":" << UTF8toEUCKR(api.GetResultMsg()) << "\n";
        return -1;
    }

    cout << "\nModifyFile ----\n";
    if(api.ModifyFileInfo(test_created_fileid, "renamed.tmp")) {
        PrintData(api);
    }
    else {
        cout << "fail:" << api.GetResultCode() << ":" << UTF8toEUCKR(api.GetResultMsg()) << "\n";
        return -1;
    }
    
    cout << "\nDownloadFile ----\n";
    string downfilename = TEST_DOWNLOADFILE;

    if(api.DownloadFile(test_created_fileid, downfilename, download_progress)) {
        cout << test_created_fileid << " : " << downfilename << "\n";
    }
    else {
        cout << "fail:" << api.GetResultCode() << ":" << UTF8toEUCKR(api.GetResultMsg()) << "\n";
        //return -1;
    }
    
    cout << "\nDeleteFile ----\n";
    if(api.DeleteFile(test_created_fileid)) {
        PrintData(api);
    }
    else {
        cout << "fail:" << api.GetResultCode() << ":" << UTF8toEUCKR(api.GetResultMsg()) << "\n";
        return -1;
    }

    cout << "\nDeleteTrash ----\n";
    if(api.DeleteTrash()) {
        PrintData(api);
    }
    else {
        cout << "fail:" << api.GetResultCode() << ":" << UTF8toEUCKR(api.GetResultMsg()) << "\n";
        return -1;
    }
    
	return 0;
}
