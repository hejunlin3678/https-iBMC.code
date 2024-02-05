import { Injectable } from '@angular/core';
import { HttpService, UserInfoService } from 'src/app/common-app/service';
import { Observable } from 'rxjs/internal/Observable';
import { IKerberos, IGroup, IGroupItem, ILoginRule, IKrbSave } from '../dataType/data.interface';
import { HttpClient, HttpErrorResponse, HttpResponse } from '@angular/common/http';
import { UserRoleList } from '../dataType/data.model';

@Injectable({
    providedIn: 'root'
})
export class KerberosService {
    constructor(
        private http: HttpService,
        private httpClient: HttpClient,
        private userInfo: UserInfoService
    ) { }

    public getRules(): Promise<ILoginRule[]> {
        return new Promise((resolve, reject) => {
            this.http.get(`/UI/Rest/AccessMgnt/LoginRule`).subscribe({
                next: (res: HttpResponse<any>) => {
                    const rules: ILoginRule[] = res.body.Members.map((item, index) => {
                        const temp: ILoginRule = {
                            id: item.ID + 1,
                            active: index === 0,
                            checked: false,
                            memberId: 'Rule' + (item.ID + 1),
                            ip: item.IP,
                            mac: item.Mac,
                            startTime: item.StartTime,
                            endTime: item.EndTime,
                            ruleEnabled: item.Status,
                        };
                        return temp;
                    });
                    resolve(rules);
                },
                error: (error: HttpErrorResponse) => {
                    reject(error);
                }
            });
        });
    }

    public getKerberos(): Observable<[IKerberos, IGroup]> {
        return new Observable((observer) => {
            this.http.get('/UI/Rest/AccessMgnt/Kerberos').subscribe((res: HttpResponse<any>) => {
                const result: IKerberos = {
                    KerberosEnabled: res.body.Enabled
                };
                const groups: IGroupItem[] = [];
                const resData = res.body;
                resData.Group.Members.forEach((item) => {
                    const loginRule: string[] = [...item.LoginRule];
                    const role = (item.RoleID === null || item.RoleID === UserRoleList.NOACCESS) ? 'Common User' : item.RoleID;
                    const groupItem: IGroupItem = {
                        groupName: item.Name === null ? '' : item.Name,
                        groupSID: item.SID === null ? '' : item.SID,
                        groupRole: role,
                        groupLoginInterface: item.LoginInterface,
                        groupLoginRule: loginRule,
                        groupDomain: item.Domain === null ? '' : item.Domain,
                        memberId: item.ID
                    };
                    groups.push(groupItem);
                });

                const group: IGroup = {
                    id: resData.Id,
                    kerberosPort: resData.Port === null ? '' : resData.Port.toString(),
                    kerberosServerAddress: resData.IPAddress,
                    name: resData.Name,
                    realm: resData.Domain,
                    uploadUrl: '',
                    kerberosGroups: groups
                };
                observer.next([result, group]);
            });
        });
    }

    // 保存Kerberos基础信息
    public saveKerberos(param: IKrbSave, pwd: string): Observable<any> {
        return new Observable((observe) => {
            if (Object.keys(param).length === 0) {
                observe.next('nochange');
                observe.complete();
            } else {
                if (pwd !== '') {
                    param.ReauthKey = window.btoa(pwd);
                }
                this.http.patch('/UI/Rest/AccessMgnt/Kerberos', param).subscribe({
                    next: () => {
                        observe.next('success');
                        observe.complete();
                    },
                    error: (error: HttpErrorResponse) => {
                        observe.error(error);
                    }
                });
            }
        });
    }

    // 上传Kerberos证书
    public uploadKrbCert(param: { secPwd: string; formData?: FormData; }): Observable<any> {
        return new Observable((observe) => {
            if (param.formData === null) {
                observe.next('nochange');
                observe.complete();
            } else {
                let header = {};
                header = {
                    headers: {
                        'X-CSRF-Token': this.userInfo.getToken(),
                        From: 'WebUI',
                        Accept: '*/*'
                    }
                };
                const url = `/UI/Rest/AccessMgnt/Kerberos/ImportKeyTab`;
                this.httpClient.post(url, param.formData, header).subscribe({
                    next: () => {
                        observe.next('success');
                        observe.complete();
                    },
                    error: (error: HttpErrorResponse) => {
                        observe.error(error);
                    }
                });
            }
        });
    }

    // Kerberos状态切换
    public switchKerberos(state: boolean): Observable<any> {
        return new Observable((observe) => {
            this.http.patch('/UI/Rest/AccessMgnt/Kerberos/State', { Enabled: state }).subscribe({
                next: (res: HttpResponse<any>) => observe.next(res),
                error: (error: HttpErrorResponse) => observe.error(error)
            });
        });
    }

    // 新增，编辑页面保存
    public saveGroup(params: any, password?: string): Observable<any> {
        return new Observable((observe) => {
            if (password !== undefined && password !== '') {
                params.ReauthKey = window.btoa(password);
            }
            this.http.patch('/UI/Rest/AccessMgnt/Kerberos', params).subscribe({
                next: (res: HttpResponse<any>) => observe.next(res),
                error: (error: HttpErrorResponse) => observe.error(error)
            });
        });
    }
}
