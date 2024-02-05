#[macro_use]
extern crate core;

use core::{body_json, multipart::body_multipart, multipart::body_multipart_save_file_in_memory};
use privilege::Privilege;
use warp;
use warp::filters::BoxedFilter;
use warp::{http::Response, Filter};
use webrest_handles as h;
use webrest_handles::{context, empty_body};

const SIZE_1K: u64 = 1024;
const SIZE_2K: u64 = 2 * 1024;
const SIZE_1M: u64 = 1024 * SIZE_1K;
const RO: Privilege = Privilege::READONLY;
const SM: Privilege = Privilege::SECURITYMGNT;
const UM: Privilege = Privilege::USERMGNT;
const BS: Privilege = Privilege::BASICSETTING;

pub fn routes() -> BoxedFilter<(Response<String>,)> {
  routes!(
    GET => {
      ["Accounts"] => (context(RO)).and_then(h::GetAccounts);
      ["LDAP"] => (context(RO)).and_then(h::GetLDAPState);
      ["LDAP" / u32] => (context(RO)).and_then(h::GetLDAP);
      ["Kerberos"] => (context(RO)).and_then(h::GetKerberos);
      ["TwoFactorAuthentication"] => (context(UM)).and_then(h::GetTwoFactorAuthentication);
      ["AdvancedSecurity"] => (context(RO)).and_then(h::GetAdvancedSecurity);
      ["LoginRule"] => (context(RO)).and_then(h::GetLoginRule);
      ["RightManagement"] => (context(RO)).and_then(h::GetRightManagement);
      ["SecurityBanner"] => (context(RO)).and_then(h::GetSecurityBanner);
      ["CertUpdateService"] => (context(RO)).and_then(h::GetCMPConfig);
      ["TPCMBasicInfo"] => (context(RO)).and_then(h::GetTPCMBaseInfo);
      ["TPCMDetailedInfo"] => (context(RO)).and_then(h::GetTPCMDetailedInfo);
      ["BootCertificates"] => (context(RO)).and_then(h::GetBootCertificates);
      ["BootCertificates" / "SecureBoot"] => (context(RO)).and_then(h::GetBiosSecureBoot);
      ["TpcmService"] => (context(RO)).and_then(h::GetTPCMTsbInfo)
    };
    PATCH => {
      ["Accounts" / u32] => (body_json(SIZE_1K), context(RO)).and_then(h::UpdateAccount);
      ["LDAP"] => (body_json(1024), context(UM)).and_then(h::SetLDAPState);
      ["LDAP" / u32] => (body_json(SIZE_1K), context(UM)).and_then(h::SetLDAP);
      ["Kerberos"] => (body_json(SIZE_1K), context(UM)).and_then(h::SetKerberos);
      ["Kerberos"  / "State"] => (body_json(1024), context(UM)).and_then(h::SetKerberosState);
      ["TwoFactorAuthentication"] => (context(UM), body_json(SIZE_1K)).and_then(h::SetTwoFactorAuthentication);
      ["AdvancedSecurity"] => (body_json(SIZE_1K), context(RO)).and_then(h::UpdateAdvancedSecurity);
      ["LoginRule"] => (body_json(SIZE_1K), context(SM)).and_then(h::UpdateLoginRule);
      ["RightManagement"] => (body_json(SIZE_1K), context(UM)).and_then(h::UpdateRightManagement);
      ["SecurityBanner"] => (body_json(SIZE_2K), context(SM)).and_then(h::UpdateSecurityBanner);
      ["CertUpdateService"] => (body_json(SIZE_2K), context(SM)).and_then(h::SetCMPConfig);
      ["BootCertificates" / "SecureBoot"] => (body_json(SIZE_2K), context(SM)).and_then(h::SetBiosSecureBoot);
      ["TpcmService"] => (context(UM), body_json(SIZE_1K)).and_then(h::SetTPCMTsbInfo);
      ["TpcmService" / "SetStandardDigest"] => (context(UM), body_json(SIZE_1K)).and_then(h::SetStandardDigest)
    };
    POST => {
      ["Accounts"] => (body_json(SIZE_1K), context(UM)).and_then(h::AddAccount);
      ["Accounts" / u32 / "ImportSSHPublicKey"] => (body_multipart(context(RO), SIZE_1M, &["pub"], h::raw::UploadFile)).and_then(h::ImportSSHPublicKey);
      ["LDAP" / u32 / "ImportLDAPCertificate"] => (body_multipart(context(UM), SIZE_1M, &["crt","cer","cert","pem"], h::raw::UploadFile)).and_then(h::ImportLDAPCertificate);
      ["LDAP" / u32 / "ImportCrlVerification"] => (body_multipart(context(UM), SIZE_1M, &["crl"], h::raw::UploadFile)).and_then(h::ImportCrlVerification);
      ["LDAP" / u32 / "DeleteCrlVerification"] => (body_json(SIZE_1K), context(RO)).and_then(h::DeleteCrlVerification);
      ["Kerberos" / "ImportKeyTab"] => (body_multipart_save_file_in_memory(context(UM), SIZE_1M, &["keytab"], h::raw::UploadFile)).and_then(h::ImportKeyTab);
      ["TwoFactorAuthentication" / "ImportClientCertificate"] => (body_multipart(context(UM), SIZE_1M, &["crt","cer","pem"], h::raw::UploadFile)).and_then(h::ImportClentCertificate);
      ["TwoFactorAuthentication" / "ImportRootCertificate"] => (body_multipart(context(UM), SIZE_1M, &["crt","cer","pem"], h::raw::UploadFile)).and_then(h::ImportRootCertificate);
      ["TwoFactorAuthentication" / "DeleteCertificate"] => (context(UM), body_json(SIZE_1K)).and_then(h::DeleteCertificate);
      ["TwoFactorAuthentication" / "ImportCrlCertificate"] => (body_multipart(context(UM), SIZE_1M, &["crl"], h::raw::UploadFile)).and_then(h::ImportCrlCertificate);
      ["TwoFactorAuthentication" / u32 / "DeleteCrlCertificate"] => (context(RO), empty_body()).and_then(h::DeleteCrlCertificate);
      ["BootCertificates" / "ImportBootCertActionInfo"] => (body_multipart(context(SM), SIZE_1M, &["crt","cer","pem"], h::raw::UploadFile)).and_then(h::ImportBootCertActionInfo);
      ["BootCertificates" / "ImportBootCrlActionInfo"] => (body_multipart(context(SM), SIZE_1M, &["crl"], h::raw::UploadFile)).and_then(h::ImportBootCrlActionInfo);
      ["BootCertificates" / "ImportSecureBootCertActionInfo"] => (body_multipart(context(SM), SIZE_1M, &["crt","cer","pem"], h::raw::UploadFile)).and_then(h::ImportSecureBootCertActionInfo);
      ["CertUpdateService" / "ImportCA"] =>(body_multipart(context(SM), SIZE_1M, &["crt","cer","pem"], h::raw::UploadFile)).and_then(h::ImportCACert);
      ["CertUpdateService" / "ImportCrl"] =>(body_multipart(context(SM), SIZE_1M, &["crl"], h::raw::UploadFile)).and_then(h::ImportCRL);
      ["CertUpdateService" / "DeleteCRL"] => (context(SM), empty_body()).and_then(h::DeleteCRL);
      ["CertUpdateService" / "ImportClientCert"] =>(body_multipart(context(SM), SIZE_1M, &["pfx","p12"], h::raw::UploadFile)).and_then(h::ImportClientCert);
      ["CertUpdateService" / "UpdateCertFromCA"] => (context(BS), empty_body()).and_then(h::UpdateCertFromCA)
    };
    DELETE => {
      ["Accounts" / u32] => (body_json(SIZE_1K), context(UM)).and_then(h::DeleteAccount);
      ["Accounts" / u32 / "DeleteSSHPublicKey"] => (body_json(SIZE_1K), context(RO)).and_then(h::DeleteSSHPublicKey)
    }
  )
  .boxed()
}
