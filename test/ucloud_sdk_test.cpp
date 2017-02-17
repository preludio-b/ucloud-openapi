size_t success_count;
size_t warn_count;
size_t fail_count;

string ok()
{
    success_count++;
    return " - ok";
}
string fail()
{
    fail_count++;
    return " - fail";
}

string warn()
{
	warn_count++;
	return " - warn";
}

void PrintData(UcloudOpenApi & api)
{
    if(api.GetResultCode().size()) cout << "result_code" << " = " << api.GetResultCode() << "\n";
    if(api.GetResultMsg().size()) cout << "result_msg" << " = " << UTF8toEUCKR(api.GetResultMsg()) << "\n";
    if(api.GetUserId().size()) cout << "userId" << " = " << api.GetUserId() << "\n";
    if(api.GetUserName().size()) cout << "userName" << " = " << UTF8toEUCKR(api.GetUserName()) << "\n";
    if(api.GetQuita().size()) cout << "quota" << " = " << api.GetQuita() << "\n";
    if(api.GetUserSpace().size()) cout << "usespace" << " = " << api.GetUserSpace() << "\n";
    
	if(api.GetFolderName().size()) cout << "folder_name" << " = " << UTF8toEUCKR(api.GetFolderName()) << "\n";
	if(api.GetFileName().size()) cout << "file_name" << " = " << UTF8toEUCKR(api.GetFileName()) << "\n";
    if(api.GetFileSize().size()) cout << "size" << " = " << api.GetFileSize() << "\n";
    if(api.GetFileModifyDate().size()) cout << "modified_date" << " = " << api.GetFileModifyDate() << "\n";
    if(api.IsFileExist()) cout << "presentOnServer" << "=" << api.IsFileExist() << "\n";
    if(api.GetFileMediaType().size()) cout << "mediaType" << " = " << api.GetFileMediaType() << "\n";
    if(api.IsPublicLink()) cout << "publicLink" << "=" << api.IsPublicLink() << "\n";
    if(api.GetCreateDate().size()) cout << "create_date" << " = " << api.GetCreateDate() << "\n";
    if(api.GetParentFolderId().size()) cout << "parent_folder_id" << " = " << api.GetParentFolderId() << "\n";
    
    for(size_t i = 0 ; i < api.GetFoldersCount() ; i++)
    {
        cout << "Folders " << i << " : " ;
		cout << "folder_name" << "=" << UTF8toEUCKR(api.GetFoldersName(i)) << ", ";
        cout << "folder_id" << "=" << api.GetFoldersId(i) << ", ";
        cout << "folder_type" << "=" << api.GetFoldersType(i) << "\n";
    }
    for(size_t i = 0 ; i < api.GetFilesCount() ; i++)
    {
        cout << "Files " << i << " : " ;
		cout << "file_name" << "=" << UTF8toEUCKR(api.GetFilesName(i)) << ", ";
        cout << "file_id" << "=" << api.GetFilesId(i) << ", ";
        cout << "file_size" << "=" << api.GetFilesSize(i) << ", ";
        cout << "modify_date" << "=" << api.GetFilesModifyDate(i) << ", ";
        cout << "presentOnServer" << "=" << api.IsFilesExist(i) << ", ";
        cout << "content_type" << "=" << api.GetFilesContentType(i) << ", ";
        cout << "action_type" << "=" << api.GetFilesLastActionType(i) << "\n";
    }
    if(api.IsHasMore())  cout << "hasMore" << " = " << api.IsHasMore() << "\n";
}

void test_urlencode()
{
    cout << "urlencode test start" << "\n";
    cout << urlencode("http://www.ucloud.com/123A") << " = urlencode('http://www.ucloud.com/123A')" << (urlencode("http://www.ucloud.com/123A").compare("http%3A%2F%2Fwww.ucloud.com%2F123A") == 0 ? ok() : fail()) << "\n";
    cout << urlencode("~!@#$%^&*()_+`-=[]{}\\ |;:'\",.<>/?") << " = urlencode('~!@#$%^&*()_+`-=[]{}\\ |;:'\",.<>/?')" << (urlencode("~!@#$%^&*()_+`-=[]{}\\ |;:'\",.<>/?").compare("~%21%40%23%24%25%5E%26%2A%28%29_%2B%60-%3D%5B%5D%7B%7D%5C%20%7C%3B%3A%27%22%2C.%3C%3E%2F%3F") == 0 ? ok() : fail()) << "\n";
    cout << "urlencode test end" << "\n";
}

void test_urldecode()
{
    cout << "urldecode test start" << "\n";
    cout << urldecode("http%3A%2F%2Fwww.ucloud.com%2F123+A") << " = urldecode('http%3A%2F%2Fwww.ucloud.com%2F123+A')" << (urldecode("http%3A%2F%2Fwww.ucloud.com%2F123+A").compare("http://www.ucloud.com/123 A") == 0 ? ok() : fail()) << "\n";
    cout << urldecode("~%21%40%23%24%25%5E%26%2A%28%29_%2B%60-%3D%5B%5D%7B%7D%5C%20%7C%3B%3A%27%22%2C.%3C%3E%2F%3F") << " = urldecode('~%21%40%23%24%25%5E%26%2A%28%29_%2B%60-%3D%5B%5D%7B%7D%5C%20%7C%3B%3A%27%22%2C.%3C%3E%2F%3F')" << (urldecode("~%21%40%23%24%25%5E%26%2A%28%29_%2B%60-%3D%5B%5D%7B%7D%5C%20%7C%3B%3A%27%22%2C.%3C%3E%2F%3F").compare("~!@#$%^&*()_+`-=[]{}\\ |;:'\",.<>/?") == 0 ? ok() : fail()) << "\n";
    cout << "urldecode test end" << "\n";
}

void test_base64_encode()
{
    cout << "base64_encode test start" << "\n";
    cout << base64_encode("~") << " = base64_encode('~')" << (base64_encode("~").compare("fg==") == 0 ? ok() : fail()) << "\n";
    cout << base64_encode("ab") << " = base64_encode('ab')" << (base64_encode("ab").compare("YWI=") == 0 ? ok() : fail()) << "\n";
    cout << base64_encode("123") << " = base64_encode('123')" << (base64_encode("123").compare("MTIz") == 0 ? ok() : fail()) << "\n";
    cout << base64_encode("한글") << " = base64_encode('한글')" << (base64_encode("한글").compare("7ZWc6riA") == 0 ? ok() : fail()) << "\n";
    cout << base64_encode("abcdefg") << " = base64_encode('abcdefg')" << (base64_encode("abcdefg").compare("YWJjZGVmZw==") == 0 ? ok() : fail()) << "\n";
    cout << "base64_encode test end" << "\n";
}

void test_get_nonce()
{
    cout << "get_nonce test start" << "\n";
    string t1 = get_nonce();
    string t2 = get_nonce();
    
    cout << "values : " << t1 << " : " << t2 << "\n";
    
    if( t1.size() > 0 && t2.size() > 0 ) {
        success_count++;
        cout << "get_nonce size - ok" << "\n";
    }
    else {
        fail_count++;
        cout << "get_nonce size - error" << "\n";
    }
  
    if( t1.compare(t2) != 0 ) {
        success_count++;
        cout << "get_nonce value - ok" << "\n";
    }
    else {
        fail_count++;
        cout << "get_nonce value - error" << "\n";
    }
    
    cout << "get_nonce test end" << "\n";
}

void test_get_fileext()
{
    cout << "get_fileext test start" << "\n";
    cout << get_fileext("") << " = get_fileext('')" << (get_fileext("").compare("") == 0 ? ok() : fail()) << "\n";
    cout << get_fileext("abc") << " = get_fileext('abc')" << (get_fileext("abc").compare("") == 0 ? ok() : fail()) << "\n";
    cout << get_fileext("abc.") << " = get_fileext('abc.')" << (get_fileext("abc.").compare("") == 0 ? ok() : fail()) << "\n";
    cout << get_fileext("abc.def") << " = get_fileext('abc.def')" << (get_fileext("abc.def").compare("def") == 0 ? ok() : fail()) << "\n";
    cout << get_fileext("aBc.dEf") << " = get_fileext('aBc.dEf')" << (get_fileext("aBc.dEf").compare("dEf") == 0 ? ok() : fail()) << "\n";
    cout << get_fileext("/abc.d/def.e/abc.def") << " = get_fileext('/abc.d/def.e/abc.def')" << (get_fileext("/abc.d/def.e/abc.def").compare("def") == 0 ? ok() : fail()) << "\n";
    cout << get_fileext("abc...def") << " = get_fileext('abc...def')" << (get_fileext("abc...def").compare("def") == 0 ? ok() : fail()) << "\n";
    cout << get_fileext(".def") << " = get_fileext('.def')" << (get_fileext(".def").compare("def") == 0 ? ok() : fail()) << "\n";
    cout << "get_fileext test end" << "\n";
}

void test_md5()
{
    cout << "md5 test start" << "\n";
    cout << hash_md5("") << " = md5('')" << (hash_md5("").compare("d41d8cd98f00b204e9800998ecf8427e") == 0 ? ok() : fail()) << "\n";
    cout << hash_md5("a") << " = md5('a')" << (hash_md5("a").compare("0cc175b9c0f1b6a831c399e269772661") == 0 ? ok() : fail()) << "\n";
    cout << hash_md5("abc") << " = md5('abc')" << (hash_md5("abc").compare("900150983cd24fb0d6963f7d28e17f72") == 0 ? ok() : fail()) << "\n";
    cout << hash_md5("message digest") << " = md5('message digest')" << (hash_md5("message digest").compare("f96b697d7cb7938d525a2f31aaf161d0") == 0 ? ok() : fail()) << "\n";
    cout << hash_md5("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789") << " = md5('ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789')" << (hash_md5("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789").compare("d174ab98d277d9f5a5611c2c9f419d9f") == 0 ? ok() : fail()) << "\n";
    cout << hash_md5("12345678901234567890123456789012345678901234567890123456789012345678901234567890") << " = md5('12345678901234567890123456789012345678901234567890123456789012345678901234567890')" << (hash_md5("12345678901234567890123456789012345678901234567890123456789012345678901234567890").compare("57edf4a22be3c955ac49da2e2107b67a") == 0 ? ok() : fail()) << "\n";
    cout << "md5 test end" << "\n";
}

void test_util()
{
    cout << "test util start" << "\n";
    test_base64_encode();
    test_urlencode();
    test_urldecode();
    test_get_nonce();
    test_get_fileext();
    test_md5();
    cout << "test util end" << "\n\n";
}

void test_login()
{
    cout << "test login start" << "\n";
    UcloudOpenApi api1;
    api1.SetApiKeys(TEST_APIKEY,TEST_APISECRETKEY);
    if(api1.XAuthLogin(TEST_USERNAME, TEST_PASSWORD)) {
        success_count++;
        cout << "login success : " << api1.GetTokenKey() << " : " << api1.GetTokenSecretKey() << " - ok\n";
    }
    else {
        fail_count++;
        cout << "login fail : " << api1.GetResultCode() << " : " << UTF8toEUCKR(api1.GetResultMsg()) << " - fail\n";
    }
    
    UcloudOpenApi api2;
    api2.SetApiKeys("MYKEY",TEST_APISECRETKEY);
    if(api2.XAuthLogin(TEST_USERNAME, TEST_PASSWORD)) {
        fail_count++;
        cout << "login success : " << api2.GetTokenKey() << " : " << api2.GetTokenSecretKey() << " - fail\n";
    }
    else {
        success_count++;
        cout << "login fail : " << api2.GetResultCode() << " : " << UTF8toEUCKR(api2.GetResultMsg()) << " - ok\n";
    }

    UcloudOpenApi api3;
    api3.SetApiKeys(TEST_APIKEY,"MYSECRET");
    if(api3.XAuthLogin(TEST_USERNAME, TEST_PASSWORD)) {
        fail_count++;
        cout << "login success : " << api3.GetTokenKey() << " : " << api3.GetTokenSecretKey() << " - fail\n";
    }
    else {
        success_count++;
        cout << "login fail : " << api3.GetResultCode() << " : " << UTF8toEUCKR(api3.GetResultMsg()) << " - ok\n";
    }

    UcloudOpenApi api4;
    api4.SetApiKeys(TEST_APIKEY,TEST_APISECRETKEY);
    if(api4.XAuthLogin("MYID", TEST_PASSWORD)) {
        fail_count++;
        cout << "login success : " << api4.GetTokenKey() << " : " << api4.GetTokenSecretKey() << " - fail\n";
    }
    else {
        success_count++;
        cout << "login fail : " << api4.GetResultCode() << " : " << UTF8toEUCKR(api4.GetResultMsg()) << " - ok\n";
    }

    UcloudOpenApi api5;
    api5.SetApiKeys(TEST_APIKEY,TEST_APISECRETKEY);
    if(api5.XAuthLogin(TEST_USERNAME, "MYPASSWORD")) {
        fail_count++;
        cout << "login success : " << api5.GetTokenKey() << " : " << api5.GetTokenSecretKey() << " - fail\n";
    }
    else {
        success_count++;
        cout << "login fail : " << api5.GetResultCode() << " : " << UTF8toEUCKR(api5.GetResultMsg()) << " - ok\n";
    }
    cout << "test login end" << "\n";
}

void test_getuserinfo()
{
    cout << "test getuserinfo start" << "\n";

    UcloudOpenApi api1;
    api1.SetApiKeys(TEST_APIKEY,TEST_APISECRETKEY);
    if(api1.XAuthLogin(TEST_USERNAME, TEST_PASSWORD)) {
        if(api1.GetUserInfo()) {
            success_count++;
            cout << "getuserinfo - xauthlogin" << " - ok\n";
            PrintData(api1);
        }
        else {
            fail_count++;
            cout << "getuserinfo fail : " << api1.GetResultCode() << " : " << UTF8toEUCKR(api1.GetResultMsg()) << " - ok\n";
        }
    }
    UcloudOpenApi api2;
    api2.SetApiKeys(TEST_APIKEY,TEST_APISECRETKEY);
    api2.SetTokenKeys(TEST_TOKENEKY, TEST_TOKENSECRETKEY);
    if(api2.GetUserInfo()) {
        success_count++;
        cout << "getuserinfo - tokenkey" << " - ok\n";
        PrintData(api2);
    }
    else {
        fail_count++;
        cout << "getuserinfo fail : " << api2.GetResultCode() << " : " << UTF8toEUCKR(api2.GetResultMsg()) << " - fail\n";
    }
    cout << "test getuserinfo end" << "\n";
}

void test_getcontents()
{
	cout << "test getcontents start" << "\n";

	UcloudOpenApi api1;
	api1.SetTokenKeys(TEST_TOKENEKY, TEST_TOKENSECRETKEY);
	api1.SetApiKeys(TEST_APIKEY,TEST_APISECRETKEY);

	if(api1.GetUserInfo()) {
		cout << "getcontents" << ok() << "\n";
		PrintData(api1);
	}
	else {
		cout << "getuserinfo fail : " << api1.GetResultCode() << " : " << UTF8toEUCKR(api1.GetResultMsg()) << fail() << "\n";
	}

	cout << "test getuserinfo end" << "\n";
}

// test and get web root folder id
string test_getsyncfolder()
{
	cout << "test getsyncfolder start" << "\n";

	UcloudOpenApi api1;
	api1.SetTokenKeys(TEST_TOKENEKY, TEST_TOKENSECRETKEY);
	api1.SetApiKeys(TEST_APIKEY,TEST_APISECRETKEY);

	if(api1.GetSyncFolder()) {
		cout << "getsyncfolder" << ok() << "\n";
		PrintData(api1);
	}
	else {
		cout << "getsyncfolder fail : " << api1.GetResultCode() << " : " << UTF8toEUCKR(api1.GetResultMsg()) << fail() << "\n";
	}

	string test_fid;

	for(size_t i = 0 ; i < api1.GetFoldersCount() ; i++)
	{
		if(api1.GetFoldersType(i).compare("Cloud") == 0) {
			test_fid = api1.GetFoldersId(i);
		}
	}

	if(test_fid.size() == 0) {
		cout << "cloud folder not found! " << warn() << "\n";
	}
	else {
		cout << "cloud folder found! " << ok() << "\n";
	}

	cout << "test getsyncfolder end" << "\n";

	return test_fid;
}

void test_openapi()
{
    string fid;
	
	cout << "test ucloudopenapi start" << "\n\n";
    test_login();
    test_getuserinfo();
	test_getcontents();
	fid = test_getsyncfolder();
    cout << "test ucloudopenapi end" << "\n\n";
}

void test()
{
    cout << "test start" << "\n\n";
    test_util();
    test_openapi();
    cout << "test end" << "\n\n";
    
    cout << "total test : " << (success_count+warn_count+fail_count) << ", ";
    cout << "success : " << success_count << ", ";
    cout << "fail : " << fail_count << ", ";
    cout << "warn : " << warn_count << "\n";
}
