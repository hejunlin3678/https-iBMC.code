import { Injectable } from '@angular/core';
import { HttpService } from 'src/app/common-app/service';

@Injectable({
    providedIn: 'root'
})
export class VncService {

    constructor(private http: HttpService) { }

    // 获取VNC配置信息
    public getVnc() {
        return this.http.get('/UI/Rest/Services/VNC');
    }

    // 设置VNC配置信息
    public setVncService(params) {
        return this.http.patch('/UI/Rest/Services/VNC', params);
    }
    public setVnc(params) {
        return new Promise((resolve, reject) => {
            if (JSON.stringify(params) !== '{}') {
                this.http.patch('/UI/Rest/Services/VNC', params).subscribe((res) => {
                    resolve(res);
                }, error => {
                    reject(error);
                });
            } else {
                resolve(false);
            }
         });
    }

    // 设置VNC配置信息
    public setVncPwd(params, pwd) {
        return new Promise((resolve, reject) => {
            if (pwd !== '') {
                params.ReauthKey = window.btoa(pwd);
                this.http.patch('/UI/Rest/Services/VNC/Password',
                    params).subscribe((res) => {
                        resolve(res);
                    }, error => {
                        reject(error);
                    });
            } else {
                resolve(false);
            }
         });
    }

    public setVncList(params: any, vncPwd: any, pwd: string): Promise<any> {
        return new Promise((resolve, reject) => {
            Promise.all([
                this.setVncPwd(vncPwd, pwd),
                this.setVnc(params),
            ]).then((resArr) => {
                resolve(resArr);
            }).catch((error) => {
                reject(error);
            });
        });
    }

    public setVncPwdWithoutPassword(params) {
        return new Promise((resolve, reject) => {
            this.http.patch('/UI/Rest/Services/VNC/Password',
                params
                ).subscribe((res) => {
                    resolve(res);
                }, error => {
                    reject(error);
                });
         });
    }

    public setVncListWithoutPassword(params: any, vncPwd: any): Promise<any> {
        return new Promise((resolve, reject) => {
            Promise.all([
                this.setVncPwdWithoutPassword(vncPwd),
                this.setVnc(params),
            ]).then((resArr) => {
                resolve(resArr);
            }).catch((error) => {
                reject(error);
            });
        });
    }

}
