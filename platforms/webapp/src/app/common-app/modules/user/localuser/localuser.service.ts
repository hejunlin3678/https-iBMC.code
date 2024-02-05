import { Injectable } from '@angular/core';
import { Observable } from 'rxjs/internal/Observable';
import { HttpClient, HttpResponse, HttpErrorResponse } from '@angular/common/http';
import { HttpService, UserInfoService } from 'src/app/common-app/service';
import { IUserInfo, ILoginRule, IError, ISaveUser, IResponseUserData, ICustError, IUsersInfo, IAddUser, IBackError, IUserId } from '../dataType/data.interface';
import { USER_ROLES, UnDeleteUsers, PasswordFailedProp } from '../dataType/data.model';
import { CommonData } from 'src/app/common-app/models';
import { PRODUCT, PRODUCTTYPE } from 'src/app/common-app/service/product.type';
import { GlobalDataService } from 'src/app/common-app/service/global-data.service';

@Injectable({
    providedIn: 'root'
})
export class LocalUserService {

    constructor(
        private http: HttpService,
        private httpClient: HttpClient,
        private userInfo: UserInfoService,
        private globalData: GlobalDataService
    ) { }

    // 获取用户列表
    public getUserList(): Observable<IUsersInfo[]> {
        return new Observable(observer => {
            Promise.all([
                this.getUsedIdList(),
                this.getPwdCheck()
            ]).then((resArr: any[]) => {
                const members = resArr[0];
                const resultArr: IUsersInfo[] = [];
                const admins = members.filter((item: IResponseUserData) => {
                    return item.RoleID === USER_ROLES.ADMINISTRATOR && item.Enabled;
                });

                members.forEach((item: IResponseUserData) => {
                    const rules: string[] = item.LoginRule.map((rule: string) => {
                        return rule.split('/').reverse().shift()[0];
                    });

                    let userInterface = item.LoginInterface;
                    if (this.userInfo.snmpSupport === false) {
                        userInterface = item.LoginInterface.filter(_item => _item !== 'SNMP');
                    }
                    const user: IUsersInfo = {
                        id: item.ID,
                        userName: item.UserName,
                        roleId: item.RoleID,
                        loginInterface: userInterface,
                        loginRole: rules,
                        validate: item.PasswordValidityDays,
                        sshPublicKeyHash: item.SSHPublicKeyHash,
                        state: item.Enabled,
                        canDeleted: true,
                        canDisabled: true,
                        canDisbledTipSHow: false,
                        unDeletedReason: '',
                        unDisabledReason: ''
                    };

                    /**
                     * 设置可删除状态和禁用启动状态
                     * 1. 只有1个管理员，紧急用户无删除，其他场景由Oem.Huawei.Deleteable值决定
                     * 2. 只有1个管理员，紧急用户无法被禁用，其他场景可以被禁用
                     * 3. 禁用的鼠标提示默认不显示
                     * 4. 当只有1个管理员时，禁用鼠标提示显示
                     */
                    if (admins.length <= 1 && item.RoleID === USER_ROLES.ADMINISTRATOR && item.Enabled) {
                        user.canDeleted = false;
                        user.canDisabled = false;
                        user.canDisbledTipSHow = true;
                        user.unDeletedReason = 'USER_UNDELETED_LAST';
                        user.unDisabledReason = 'USER_UNDELETED_LAST';
                    } else {
                        // DelDisableReason可能为null, 也可能是具体提示原因
                        const delDisableReason = item.DelDisableReason;
                        if (delDisableReason) {
                            if (delDisableReason === UnDeleteUsers.SSHLOGIN) {
                                user.unDeletedReason = '';
                            } else if (delDisableReason === UnDeleteUsers.TRAPV3USER) {
                                user.unDeletedReason = 'USER_UNDELETED_TRAPV3';
                            } else if (delDisableReason === UnDeleteUsers.EMERGENCYUSER) {
                                user.unDeletedReason = 'USER_UNDELETED_URGENT';
                            } else {
                                user.unDeletedReason = '';
                            }
                        }

                        // 如果是紧急登录用户，则不能删除，不能禁用
                        const emergencyUser = resArr[1].emergencyLoginUser;
                        if (emergencyUser === item.UserName) {
                            user.canDeleted = false;
                            user.canDisabled = false;
                            user.deleteableTip = 'USER_UNDELETED_LAST';
                        } else {
                            user.canDeleted = item.Deleteable;
                        }
                    }

                    resultArr.push(user);
                });
                observer.next(resultArr);
            }).catch((error) => {
                observer.error(error);
            });
        });
    }

    // 查询用户列表
    public getUsedIdList(): Promise<IResponseUserData[]> {
        return new Promise((resolve, reject) => {
            this.http.get('/UI/Rest/AccessMgnt/Accounts').subscribe({
                next: (res: HttpResponse<any>) => {
                    resolve(res.body.Members);
                },
                error: (error: HttpErrorResponse) => {
                    reject(error);
                }
            });
        });
    }

    // 查询是否开启了用户密码校验和查询紧急用户
    public getPwdCheck(): Promise<{ pwdCheck: boolean, emergencyLoginUser: string, minPwdLength: number }> {
        return new Promise((resolve, reject) => {
            this.http.get('/UI/Rest/AccessMgnt/AdvancedSecurity').subscribe({
                next: (res: HttpResponse<any>) => {
                    resolve({
                        pwdCheck: res.body.PasswordComplexityCheckEnabled,
                        emergencyLoginUser: res.body.EmergencyLoginUser,
                        minPwdLength: res.body.MinimumPasswordLength
                    });
                },
                error: (error: HttpErrorResponse) => {
                    reject(error);
                }
            });
        });
    }
    // 获取规则项列表
    public getRules(): Promise<ILoginRule[]> {
        return new Promise((resolve, reject) => {
            this.http.get('/UI/Rest/AccessMgnt/LoginRule').subscribe({
                next: (res: HttpResponse<any>) => {
                    const ruleArr: ILoginRule[] = [];
                    res.body.Members.forEach((item: { [key: string]: any }, index: number) => {
                        const ruleItem: ILoginRule = {
                            id: item.ID + 1,
                            checked: false,
                            ruleEnabled: item.Status,
                            startTime: item.StartTime,
                            endTime: item.EndTime,
                            ip: item.IP,
                            mac: item.Mac,
                            active: index === 0 ? true : false,
                        };
                        ruleArr.push(ruleItem);
                    });
                    resolve(ruleArr);
                },
                error: (error: HttpErrorResponse) => {
                    reject(error);
                }
            });
        });
    }

    // 查询特定用户相关信息
    public getUserInfo(id: number): Promise<IUserInfo> {
        return new Promise((resolve, reject) => {
            this.getUsedIdList().then((res: IResponseUserData[]) => {
                const userInfo = res.filter((item: IResponseUserData) => {
                    if (item.ID === id) {
                        return item;
                    }
                })[0];
                const user: IUserInfo = {
                    RoleId: userInfo?.RoleID,
                    LoginInterface: userInfo?.LoginInterface,
                    LoginRule: userInfo?.LoginRule,
                    UserName: userInfo?.UserName,
                    AuthProtocol: userInfo?.SnmpV3AuthProtocol || 'SHA256',
                    EncryProtocol: userInfo?.SnmpV3PrivProtocol || 'AES',
                    SNMPEncryptPwdInit: userInfo?.SNMPEncryptPwdInit,
                    FirstLoginPolicy: userInfo?.FirstLoginPolicy
                };
                resolve(user);

            }).catch(error => reject(error));
        });
    }

    // 获取添加用户时的相关信息
    public getAddUserParams(): Observable<any> {
        return new Observable((observer) => {
            Promise.all([
                this.getUsedIdList(),
                this.getPwdCheck(),
                this.getRules()
            ]).then((resArr: any[]) => {
                const idList: string[] = ['2', '3', '4', '5', '6', '7', '8', '9', '10', '11', '12', '13', '14', '15', '16', '17'];
                const members = resArr[0] || [];
                const tempArr = [];
                let firstLoginResetSupport = false;
                // 去除已经存在的用户ID
                members.forEach((item) => {
                    if (!firstLoginResetSupport && item.FirstLoginPolicy !== undefined) {
                        firstLoginResetSupport = true;
                    }
                    const position: number = idList.indexOf(String(item.ID));
                    if (position > -1) {
                        idList.splice(position, 1);
                    }
                });
                idList.forEach((item) => {
                    const tempId: IUserId = {
                        id: parseInt(item, 10),
                        label: item.toString()
                    };
                    tempArr.push(tempId);
                });

                // 登录规则
                const rules: ILoginRule[] = resArr[2];

                // 是否开启密码检查
                const pwdCheck = resArr[1].pwdCheck;

                // 设置的最小密码长度
                const minPwdLength = resArr[1].minPwdLength;
                const result = {
                    pwdCheck,
                    minPwdLength,
                    userIdList: tempArr,
                    rules,
                    firstLoginResetSupport
                };
                observer.next(result);
            }).catch((error: HttpErrorResponse) => {
                observer.error(error);
            });
        });
    }

    // 获取编辑用户时相关信息
    public getEditUserParams(userId: number): Observable<any> {
        return new Observable((observer) => {
            Promise.all([
                this.getPwdCheck(),
                this.getRules(),
                this.getUserInfo(userId)
            ]).then((resArr) => {
                // 盘古密码修改策略：0，1，2 （盘古只保留强制修改密码，返回0和1时统一为2的强制修改选项）
                if ( this.globalData.productType === PRODUCTTYPE.PANGEA &&
                    (resArr[2].FirstLoginPolicy === 0 || resArr[2].FirstLoginPolicy === 1)) {
                    resArr[2].FirstLoginPolicy = 2;
                }
                // 密码修改策略：0，1，2 （0是用户自定义修改的，在前端与1的功能一致，统一为提示修改。）
                if (resArr[2].FirstLoginPolicy === 0) {
                    resArr[2].FirstLoginPolicy = 1;
                }
                // 登录规则
                const rules: ILoginRule[] = resArr[1];

                // 是否开启密码检查
                const pwdCheck = resArr[0].pwdCheck;
                // 设置的最小密码长度
                const minPwdLength = resArr[0].minPwdLength;

                // 获取用户信息,主要包括rule,role,interface,
                const userInfo = {
                    roleId: resArr[2].RoleId,
                    interfaceList: resArr[2].LoginInterface,
                    loginRule: resArr[2].LoginRule,
                    userName: resArr[2].UserName,
                    authProtocol: resArr[2].AuthProtocol,
                    encryProtocol: resArr[2].EncryProtocol,
                    firstLoginPolicy: resArr[2].FirstLoginPolicy
                };
                const snmpv3PwdInit = resArr[2].SNMPEncryptPwdInit;
                const result = {
                    pwdCheck,
                    minPwdLength,
                    rules,
                    userInfo,
                    snmpv3PwdInit
                };
                observer.next(result);
            });
        });
    }

    public addUser(param: IAddUser, password: string): Observable<ICustError> {
        /**
         * 新增用户的逻辑如下
         * 1.对ID为参数值的对象赋值密码，角色，用户名
         * 2.对ID为参数值的用户设置规则和接口
         * 3.2步骤必须等到1步骤操作成功后才执行
         */
        return new Observable((observer) => {
            const params: { [key: string]: any } = {
                ID: param.Id,
                UserName: param.UserName,
                Password: param.Password,
                RoleID: param.RoleId,
                FirstLoginPolicy: param.FirstLoginPolicy
            };
            if (param.LoginInterface.length > 0) {
                params.LoginInterface = param.LoginInterface;
            }
            if (param.LoginRule.length > 0) {
                params.LoginRule = param.LoginRule;
            }
            if (password !== '') {
                params.ReauthKey = window.btoa(password);
            }
            this.http.post(`/UI/Rest/AccessMgnt/Accounts`, params).subscribe({
                next: () => {
                    observer.next({
                        type: 'success',
                        errors: null
                    });
                },
                error: (error) => {
                    const custError: IError[] = this.packingErrorMsg(error.error.error);
                    observer.error({
                        type: 'error',
                        errors: custError
                    });
                }
            });
        });
    }

    // 编辑用户
    public editUser(param: ISaveUser, password: string): Observable<ICustError> {
        return new Observable((observer) => {
            // 保存用户信息
            this.updateAccountInfo(param, password).then((res) => {
                let result: ICustError = null;
                if (res.body.error) {
                    const custError: IError[] = this.packingErrorMsg(res.body.error);
                    result = { type: 'someFailed', errors: custError };
                } else {
                    result = { type: 'success', errors: null };
                }
                observer.next(result);
            }).catch((error) => {
                const custError: IError[] = this.packingErrorMsg(error.error.error);
                observer.error({ type: 'error', errors: custError });
            });
        });
    }

    // 更新用户基本信息
    public updateAccountInfo(param: ISaveUser, password: string): Promise<any> {
        return new Promise((resolve, reject) => {
            const url = `/UI/Rest/AccessMgnt/Accounts/${param.Id}`;
            const params: { [key: string]: any } = {
                'Password': param.Password,
                'RoleID': param.RoleId,
                'UserName': param.UserName
            };
            if (param.LoginInterface) {
                params.LoginInterface = param.LoginInterface;
            }
            if (param.LoginRule) {
                params.LoginRule = param.LoginRule;
            }
            if (param.SNMPV3Password) {
                params.SNMPV3PrivPasswd = param.SNMPV3Password;
            }
            if (param.SnmpV3AuthProtocol) {
                params.SnmpV3AuthProtocol = param.SnmpV3AuthProtocol;
            }
            if (param.SnmpV3PrivProtocol) {
                params.SnmpV3PrivProtocol = param.SnmpV3PrivProtocol;
            }
            if (param.FirstLoginPolicy) {
                params.FirstLoginPolicy = param.FirstLoginPolicy;
            }

            // 去除无效属性
            for (const key in params) {
                if (Object.prototype.hasOwnProperty.call(params, key)) {
                    const value = params[key];
                    if (value === undefined) {
                        delete params[key];
                    }
                }
            }

            // 开始保存数据
            if (this.isEmpty(params)) {
                resolve(null);
            } else {
                if (password !== '') {
                    params.ReauthKey = window.btoa(password);
                }
                this.http.patch(url, params).subscribe({
                    next: (res) => {
                        resolve(res);
                    },
                    error: (error) => {
                        reject(error);
                    }
                });
            }
        });
    }

    // 删除用户
    public deleteUser(userId: string, password: string): Observable<ICustError> {
        return new Observable((observer) => {
            const url = `/UI/Rest/AccessMgnt/Accounts/${userId}`;
            let params = {};
            if (password !== '') {
                params = {
                    ReauthKey: window.btoa(password)
                };
            }
            this.http.delete(url, {}, params).subscribe({
                next: () => {
                    observer.next({
                        type: 'success',
                        errors: null
                    });
                },
                error: (error) => {
                    const custError: IError[] = this.packingErrorMsg(error.error.error);
                    observer.error({ type: 'error', errors: custError });
                }
            });
        });
    }

    // 禁用用户
    public modifyUserState(userId: string, password: string, state: boolean): Observable<ICustError> {
        return new Observable((observer) => {
            const url = `/UI/Rest/AccessMgnt/Accounts/${userId}`;
            const params: { [key: string]: any } = {
                Enabled: state,
            };
            if (password !== '') {
                params.ReauthKey = window.btoa(password);
            }
            this.http.patch(url, params).subscribe({
                next: () => {
                    observer.next({
                        type: 'success',
                        errors: null
                    });
                },
                error: (error) => {
                    const custError: IError[] = this.packingErrorMsg(error.error.error);
                    observer.error({ type: 'error', errors: custError });
                }
            });
        });
    }

    // 判断对象是否为空对象
    private isEmpty(o: { [key: string]: any }): boolean {
        let state = true;
        Object.keys(o).forEach(() => {
            state = false;
        });
        return state;
    }

    // 上传SSH公钥
    public uploadFile(param: { userId: number; secPwd: string; formData?: FormData; }): Observable<ICustError> {
        return new Observable((observer) => {
            const url = `/UI/Rest/AccessMgnt/Accounts/${param.userId}/ImportSSHPublicKey`;
            let header = {};
            header = {
                headers: {
                    'X-CSRF-Token': this.userInfo.getToken(),
                    From: 'WebUI',
                    Accept: '*/*'
                }
            };
            this.httpClient.post(url, param.formData, header).subscribe({
                next: () => {
                    observer.next({
                        type: 'success',
                        errors: null
                    });
                },
                error: (error) => {
                    observer.error(error);
                }
            });
        });
    }

    // 删除SSH公钥
    public deleteSSHKey(userId: string, password: string): Observable<ICustError> {
        return new Observable((observer) => {
            const url = `/UI/Rest/AccessMgnt/Accounts/${userId}/DeleteSSHPublicKey`;
            let params = {};
            if (password !== '') {
                params = {
                    ReauthKey: window.btoa(password)
                };
            }
            this.http.delete(url, {}, params).subscribe({
                next: () => {
                    observer.next({
                        type: 'success',
                        errors: null
                    });
                },
                error: (error) => {
                    const custError: IError[] = this.packingErrorMsg(error.error.error);
                    observer.error({ type: 'error', errors: custError });
                }
            });
        });
    }

    private packingErrorMsg(error: IBackError[]): IError[] {
        // 封装的错误对象结构为 [{errorId: 'id1', relatedProp: 'Password'}]
        const extendedInfo = error;
        const errorData: IError[] = [];
        extendedInfo.forEach((errorItem) => {
            const errorId = errorItem.code;
            let relatedProp = '';
            if (errorItem.relatedProperties && errorItem.relatedProperties[0]) {
                relatedProp = errorItem.relatedProperties[0].slice(2);
            }
            // 该段代码只是暂时的,后期master后端代码反合后就不需要了，因为master上密码错误时RelatedProperties为空数组，因此，当同时保存密码和V3密码时，无法区分出到底是密码还是V3密码错误
            if (errorItem.message && errorItem.message.indexOf(PasswordFailedProp.PASSWORDPROP) > -1) {
                relatedProp = PasswordFailedProp.PASSWORD;
            }
            if (errorItem.message && errorItem.message.indexOf(PasswordFailedProp.SNMPV3PASSWORDPROP) > -1) {
                relatedProp = PasswordFailedProp.SNMPV3PASSWORD;
            }
            const temp: IError = {
                errorId,
                relatedProp
            };
            errorData.push(temp);
        });
        return errorData;
    }
}
