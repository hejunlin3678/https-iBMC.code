import { Injectable } from '@angular/core';
import { Observable } from 'rxjs/internal/Observable';
import { ILdapControllerItem, ILdapGroup } from '../dataType/data.interface';
import { HttpClient, HttpResponse, HttpErrorResponse } from '@angular/common/http';
import { HttpService, UserInfoService } from 'src/app/common-app/service';
import { ILdapController, ILdapCert, ILdapCertSigle, ILdapGroupItem } from '../dataType/data.interface';

@Injectable({
    providedIn: 'root'
})
export class LdapService {
    constructor(
        private http: HttpService,
        private httpClient: HttpClient,
        private userInfo: UserInfoService
    ) { }

    public getLdapBase(): Observable<boolean> {
        return new Observable((observe) => {
            this.http.get('/UI/Rest/AccessMgnt/LDAP').subscribe((res: HttpResponse<any>) => {
                const ldapEnabled: boolean = res.body.Enabled;
                observe.next(ldapEnabled);
            }, (error: HttpErrorResponse) => {
                observe.error(error);
            });
        });
    }

    public getLdapControllItem(controlId: number): Observable<any> {
        return new Observable((observe) => {
            this.http.get('/UI/Rest/AccessMgnt/LDAP/' + controlId).subscribe((res: HttpResponse<any>) => {
                const data = res.body;
                const groups: ILdapGroup[] = [];
                data.LdapGroup.Members.forEach((item: ILdapGroupItem) => {
                    groups.push({
                        memberId: item.ID,
                        groupName: item.Name === null ? '' : item.Name,
                        groupFolder: item.Folder === null ? '' : item.Folder,
                        groupRole: item.RoleID === null ? '' : item.RoleID,
                        groupLoginRule: item.LoginRule,
                        groupLoginInterface: [...item.LoginInterface]
                    });
                });

                const cotrollerInfo: ILdapController = data.Controller;
                const certificate: ILdapCert = data.Certificate;
                const controllerItem: ILdapControllerItem = {
                    ldapServerAddress: cotrollerInfo.IPAddress === null ? '' : cotrollerInfo.IPAddress,
                    ldapPort: cotrollerInfo.Port === null ? '' : String(cotrollerInfo.Port),
                    bindDN: cotrollerInfo.BindDN === null ? '' : cotrollerInfo.BindDN,
                    bindPassword: cotrollerInfo.BindPwd === null ? '' : cotrollerInfo.BindPwd,
                    userFolder: cotrollerInfo.Folder,
                    domain: cotrollerInfo.Domain,
                    certificateVerificationEnabled: certificate.VerificationEnabled,
                    certificateVerificationLevel: certificate.VerificationLevel,
                    importCert: `/UI/Rest/AccessMgnt/LDAP/${controlId}/ImportLDAPCertificate`,
                    importCrl: `/UI/Rest/AccessMgnt/LDAP/'${controlId}/ImportCrlVerification`,
                    deleteCrl: `/UI/Rest/AccessMgnt/LDAP/'${controlId}/DeleteCrlVerification`,
                    LdapGroups: groups,
                    cerInfo: {}
                };
                const serverCert: ILdapCertSigle = certificate.ServerCert;
                const middleCert: ILdapCertSigle[] = certificate.IntermediateCert;
                const rootCert: ILdapCertSigle = certificate.RootCert;
                // 服务器证书域展示
                if (serverCert) {
                    let validTime = '';
                    if (certificate.CRLValidNotBefore && certificate.CRLValidNotAfter) {
                        validTime = `${certificate.CRLValidNotBefore} - ${certificate.CRLValidNotAfter}`;
                    }

                    controllerItem.cerInfo.server = {
                        type: 'server',
                        title: 'serverCert',
                        label: 'LDPA_SERVER_CERTIFY_INFO',
                        info: {
                            IssueBy: serverCert.Issuer,
                            IssueTo: serverCert.Subject,
                            SerialNumber: serverCert.SerialNumber,
                            ValidTime: serverCert.ValidNotBefore + ' - ' + serverCert.ValidNotAfter,
                            crlState: certificate.CrlVerification,
                            crlValidTime: validTime
                        }
                    };
                }
                // 中间证书域展示
                if (middleCert && middleCert.length > 0) {
                    const middleCerts = [];
                    middleCert.forEach((obj) => {
                        middleCerts.push({
                            type: 'middle',
                            IssueBy: obj.Issuer,
                            IssueTo: obj.Subject,
                            SerialNumber: obj.SerialNumber,
                            ValidTime: obj.ValidNotBefore + ' - ' + obj.ValidNotAfter
                        });
                    });

                    controllerItem.cerInfo.middle = {
                        type: 'middle',
                        title: 'middleCerts',
                        label: 'SERVICE_WBS_MIDDLE_CRE_INFO',
                        info: middleCerts
                    };
                }
                // 根证书域展示
                if (rootCert) {
                    controllerItem.cerInfo.root = {
                        type: 'root',
                        title: 'rootCert',
                        label: 'SERVICE_WBS_ROOT_CRE_INFO',
                        info: {
                            IssueBy: rootCert.Issuer,
                            IssueTo: rootCert.Subject,
                            SerialNumber: rootCert.SerialNumber,
                            ValidTime: rootCert.ValidNotBefore + ' - ' + rootCert.ValidNotAfter
                        }
                    };
                }
                observe.next(controllerItem);
            });
        });
    }

    public saveGroup(controlId, params, password?): Observable<any> {
        return new Observable((observe) => {
            if (password !== undefined && password !== '') {
                params.ReauthKey = window.btoa(password);
            }
            this.http.patch('/UI/Rest/AccessMgnt/LDAP/' + controlId, params).subscribe({
                next: (res) => {
                    observe.next(res);
                },
                error: (error) => {
                    observe.error(error);
                }
            });
        });
    }

    // 获取登录规则
    public getRules(): Promise<any> {
        return new Promise((resolve, reject) => {
            this.http.get(`/UI/Rest/AccessMgnt/LoginRule`).subscribe((res) => {
                const rules = [];
                const loginRules = res.body.Members;
                loginRules.forEach((item, index) => {
                    rules[index] = {};
                    rules[index].id = index + 1;
                    rules[index].checked = false;
                    rules[index].memberId = 'Rule' + parseInt(item.ID + 1, 10);
                    rules[index].ruleEnabled = item.Status;
                    rules[index].startTime = item.StartTime;
                    rules[index].endTime = item.EndTime;
                    rules[index].ip = item.IP;
                    rules[index].mac = item.Mac;
                    rules[index].active = index === 0 ? true : false;
                });
                resolve(rules);
            }, (error: HttpErrorResponse) => {
                reject(error);
            });
        });
    }
    public invalidCerUpload(url: string) {
        return new Observable((observe) => {
            this.http.get(url).subscribe((res) => {
                const members = res.body.Members.map((item) => {
                    return item['@odata.id'];
                });
                observe.next(members);
            }, (error: HttpErrorResponse) => {
                observe.error(error);
            });
        });
    }

    // 开启和关闭LDAP使能
    public switchLdap(value): Observable<any> {
        return new Observable((observe) => {
            this.http.patch(`/UI/Rest/AccessMgnt/LDAP`, {
                Enabled: value
            }).subscribe({
                next: (res) => {
                    observe.next(res);
                },
                error: (error) => {
                    observe.error(error);
                }
            });
        });
    }

    // 保存LDAP参数
    public saveLdap(controlId: number, params: any, password: string): Observable<any> {
        const controlParam = {
            BindDN: params.bddn,
            BindPwd: params.bdpwd,
            Domain: params.domain,
            Folder: params.folder,
            Port: params.port,
            IPAddress: params.server
        };
        const certParam = {
            VerificationEnabled: params.certificateVerificationEnabled,
            VerificationLevel: params.certificateVerificationLevel,
        };

        // 去除值无效的
        Object.keys(controlParam).forEach((key) => {
            if (controlParam[key] === undefined) {
                delete controlParam[key];
            }
        });
        // 去除值无效的
        Object.keys(certParam).forEach((key) => {
            if (certParam[key] === undefined) {
                delete certParam[key];
            }
        });
        const sendParam = {
            Controller: controlParam,
            Certificate: certParam
        };

        if (password !== '') {
            sendParam['ReauthKey'] = window.btoa(password);
        }

        Object.keys(sendParam).forEach((key) => {
            if (JSON.stringify(sendParam[key]) === '{}') {
                delete sendParam[key];
            }
        });

        return new Observable((observe) => {
            this.http.patch(`/UI/Rest/AccessMgnt/LDAP/${controlId}`, sendParam).subscribe({
                next: (res) => {
                    observe.next(res);
                },
                error: (error) => {
                    observe.error(error);
                }
            });
        });
    }

    // 上传证书文件, 该接口这里只返回流
    public uploadFile(param: { userId: string; content: FormData; pwd: string }): Observable<any> {
        const url = `/UI/Rest/AccessMgnt/LDAP/${param.userId}/ImportCrlVerification`;
        let header = {};
        header = {
            headers: {
                'X-CSRF-Token': this.userInfo.getToken(),
                From: 'WebUI',
                Accept: '*/*'
            }
        };
        return this.httpClient.post(url, param.content, header);
    }
    // 删除吊销证书文件
    public deleteCrlVerification(id: number, password: string): Observable<any> {
        // 组装参数
        const params: {ReauthKey: string} = {ReauthKey: ''};
        if (password !== undefined && password !== '') {
            params.ReauthKey = window.btoa(password);
        }
        const url = `/UI/Rest/AccessMgnt/LDAP/${id}/DeleteCrlVerification`;
        return this.http.post(url, params);
    }
}
