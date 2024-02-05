import { Injectable } from '@angular/core';
import { IRustPrivil } from './model/loginType.model';
import { catchError } from 'rxjs/internal/operators/catchError';
import { of } from 'rxjs/internal/observable/of';
import { Observable } from 'rxjs/internal/Observable';
import { pluck } from 'rxjs/internal/operators/pluck';
import { HttpService } from 'src/app/common-app/service';

@Injectable({
    providedIn: 'root'
})
export class LoginService {

    constructor(private http: HttpService) { }

    public login(param) {
        return this.http.post('/UI/Rest/Login', param, { ignoredErrorTip: true }).pipe(
            pluck('body')
        );
    }

    public loginOut(id: string) {
        return this.http.delete(`/UI/Rest/Sessions/${id}`).pipe(
            catchError(() => {
                return of(true);
            })
        );
    }

    public kvmLogin(param) {
        return this.http.post('/UI/Rest/KVMHandler', param, { ignoredErrorTip: true });
    }

    public getKVMHandler(url) {
        return this.http.get(url).pipe(
            pluck('body')
        );
    }

    public getPrivilegs(roles: string[]): Observable<any> {
        return new Observable((observe) => {
            this.http.get(`/UI/Rest/AccessMgnt/RightManagement`).subscribe({
                next: (res: { Count: number; Members: any[] }) => {
                    const privilList: IRustPrivil[] = res['body'].Members;
                    const relations = {
                        QueryEnabled: 'Login',
                        BasicConfigEnabled: 'ConfigureComponents',
                        ConfigureSelfEnabled: 'ConfigureSelf',
                        UserConfigEnabled: 'ConfigureUsers',
                        RemoteControlEnabled: 'OemKvm',
                        SecurityConfigEnabled: 'OemSecurityMgmt',
                        PowerControlEnabled: 'OemPowerControl',
                        VMMEnabled: 'OemVmm',
                        DiagnosisEnabled: 'OemDiagnosis'
                    };

                    const rolePrivils = [];
                    roles.forEach((roleName) => {
                        const privilItems = privilList.filter((item) => {
                            return item.Name === roleName;
                        })[0];
                        rolePrivils.push(privilItems);
                    });

                    let rolePrivil: IRustPrivil = {};
                    rolePrivils.forEach((privilItem, index) => {
                        if (index === 0) {
                            rolePrivil = privilItem;
                        } else {
                            Object.keys(privilItem).forEach((privilKey) => {
                                rolePrivil[privilKey] = rolePrivil[privilKey] || privilItem[privilKey];
                            });
                        }
                    });

                    const userPrivil = [];
                    Object.keys(rolePrivil).forEach((key) => {
                        if (relations[key] !== undefined && rolePrivil[key] === true) {
                            userPrivil.push(relations[key]);
                        }
                    });
                    observe.next(userPrivil);
                },
                error: (error) => {
                    observe.error(error);
                }
            });
        });
    }

    public getV1Info() {
        return this.http.get('/UI/Rest/Login');
    }

    public modifyUserInfo(userId, param, pwd): Observable<any> {
        return new Observable(observe => {
            if (pwd !== '') {
                param.ReauthKey = window.btoa(pwd);
            }
            this.http.patch(`/UI/Rest/AccessMgnt/Accounts/${userId}`, param).subscribe({
                next: (res) => {
                    observe.next(res);
                },
                error: (error) => {
                    observe.error(error);
                }
            });
        });
    }

    // 双因素登录
    public getTwoFactor(): Observable<any> {
        return new Observable((observe) => {
            this.http.get(`/UI/Rest/MutualHandler`).subscribe({
                next: (res) => {
                    // state状态： session:'', stateCode: 0、1、4、其他 ,token:''
                    const state = res['body'];
                    observe.next(state);
                },
                error: (error) => {
                    observe.error(error);
                }
            });
        });
    }

    // kerberos 单点登录
    public getSSOKerberos() {
        return this.http.get('/UI/Rest/KerberosHandler', {
            headers: {
                'Content-Type': 'application/text;charset=utf-8'
            },
            responseType: 'text'
        });
    }

    // 查询Session信息，
    public getSessions(): Observable<any> {
        return new Observable((observe) => {
            this.http.get(`/UI/Rest/Sessions`).subscribe((res) => {
                observe.next(res['body']);
            });
        });
    }

    // 查询是否开启了密码检查
    public getPwdChecked(): Observable<any> {
        return new Observable((observe) => {
            this.http.get(`/UI/Rest/AccessMgnt/AdvancedSecurity`).subscribe({
                next: (res) => {
                    const check = res['body'].PasswordComplexityCheckEnabled;
                    observe.next(check);
                },
                error: (error) => {
                    // 如果接口失败了，则默认按开启密码检查处理
                    observe.next(true);
                }
            });
        });
    }
}
