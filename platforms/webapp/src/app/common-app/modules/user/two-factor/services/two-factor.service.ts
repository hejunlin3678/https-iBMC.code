import { Injectable, EventEmitter } from '@angular/core';
import { HttpService } from 'src/app/common-app/service';
import { TranslateService } from '@ngx-translate/core';

@Injectable({
    providedIn: 'root'
})
export class TwoFactorService {
    public eventEmit = new EventEmitter();

    constructor(
        private http: HttpService,
        private translate: TranslateService
    ) { }

    // 获取双因素认证配置信息
    public getTwoFactoryData() {
        return this.http.get('/UI/Rest/AccessMgnt/TwoFactorAuthentication');
    }

    // 设置双因素认证配置信息
    public setTwoFactoryData(data) {
        return this.http.patch('/UI/Rest/AccessMgnt/TwoFactorAuthentication', data);
    }

    // 删除根证书/客户端证书
    public delRootCert(param) {
        return this.http.post('/UI/Rest/AccessMgnt/TwoFactorAuthentication/DeleteCertificate', param);
    }

    // 删除CRL证书
    public delCrlVerification(id: number) {
        return this.http.post(`/UI/Rest/AccessMgnt/TwoFactorAuthentication/${id}/DeleteCrlCertificate`, {});
    }

    public rootCertInfoAssemble(data) {
        const rootCertificate = data.RootCertificateMembers;
        const certificates = [];
        if (JSON.stringify(rootCertificate) !== '{}') {
            for (const item of rootCertificate) {
                const issueByTarget = item.IssueBy;
                const issueByCNTarget = issueByTarget.split(',')[0].split('=')[1];
                const issueToTarget = item.IssueTo;
                const issueToCNTarget = issueToTarget.split(',')[0].split('=')[1];
                let crlValidFromTo = '--';
                if (item.CrlValidFrom && item.CrlValidTo) {
                    crlValidFromTo = item.CrlValidFrom + ' - ' + item.CrlValidTo;
                }
                let validFromTo = '--';
                if (item.ValidFrom && item.ValidTo) {
                    validFromTo = item.ValidFrom + ' - ' + item.ValidTo;
                }
                certificates.push({
                    certId: item.ID,
                    issueBy: issueByTarget,
                    issueByCN: issueByCNTarget,
                    issueTo: issueToTarget,
                    issueToCN: issueToCNTarget,
                    validFrom: item.ValidFrom,
                    validTo: item.ValidTo,
                    valid: validFromTo,
                    crlValidFrom: item.CrlValidFrom,
                    crlValidTo: item.CrlValidTo,
                    crlValid: crlValidFromTo,
                    isImportCrl: item.CrlState,
                    serialNumber: item.SerialNumber,
                    isCRLUpload: false
                });
            }
        }
        return certificates;
    }
    private getRoleName(id: string) {
        let roleName = '';
        switch (id) {
            case 'Administrator':
                roleName = this.translate.instant('COMMON_ROLE_ADMINISTRATOR');
                break;
            case 'Operator':
                roleName = this.translate.instant('COMMON_ROLE_OPERATOR');
                break;
            case 'CommonUser':
                roleName = this.translate.instant('COMMON_ROLE_COMMONUSER');
                break;
            case 'NoAccess':
                roleName = this.translate.instant('COMMON_ROLE_NOACCESS');
                break;
            case 'CustomRole1':
                roleName = this.translate.instant('COMMON_ROLE_CUSTOMROLE1');
                break;
            case 'CustomRole2':
                roleName = this.translate.instant('COMMON_ROLE_CUSTOMROLE2');
                break;
            case 'CustomRole3':
                roleName = this.translate.instant('COMMON_ROLE_CUSTOMROLE3');
                break;
            case 'CustomRole4':
                roleName = this.translate.instant('COMMON_ROLE_CUSTOMROLE4');
                break;
        }
        return roleName;
    }

    // 客户端证书查询
    public clientCertInfoAssemble(data) {
        const clientCertData = [];
        for (const item of data.ClientCertificateMembers) {
            let fingerPrint = '--';
            let revokedTime = '--';
            let revokedStatus = null;
            let uploadState = true;
            let rootState = this.translate.instant('COMMON_NOT_UPLOADED');
            let revokedStatusValue = this.translate.instant('COMMON_NOT_UPLOADED');
            let roleID = '';
            const next = item;
            if (next) {
                if (next.RootCertUploadedState) {
                    rootState = this.translate.instant('COMMON_UPLOADED');
                } else {
                    rootState = this.translate.instant('COMMON_NOT_UPLOADED');
                }
                if (next.FingerPrint !== null && next.FingerPrint !== '') {
                    fingerPrint = next.FingerPrint;
                    uploadState = false;
                }
                // 证书已吊销情况下，吊销时间也可能为 null , 未吊销的情况下，吊销时间为 null
                revokedTime = next.RevokedDate != null ? next.RevokedDate : '--';
                revokedStatus = next.RevokedState;
                if (next.RevokedState) {
                    revokedStatusValue = this.translate.instant('TWO_FACTORS_REVOKED');
                } else {
                    revokedStatusValue = this.translate.instant('TWO_FACTORS_UNREVOKED');
                }
                roleID = next.RoleID.replace(/\s+/g, '');
            }

            const certInfo = {
                id: item.UserID,
                userName: item.UserName,
                usered: this.getRoleName(roleID),
                issueBycertificateStatus: rootState,
                clientCertificateFingerprint: fingerPrint,
                revokedState: revokedStatus,
                revokedStateValue: revokedStatusValue,
                revokedDate: revokedTime,
                isUpload: uploadState,
                isClientUpload: false
            };
            clientCertData.push(certInfo);
        }
        return clientCertData;
    }
}
