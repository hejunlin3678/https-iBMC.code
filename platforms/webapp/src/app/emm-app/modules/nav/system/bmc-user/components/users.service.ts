import { HttpErrorResponse, HttpResponse } from '@angular/common/http';
import { Injectable } from '@angular/core';
import { Observable } from 'rxjs/internal/Observable';
import { catchError, map, pluck } from 'rxjs/operators';
import { IBackRedfishError, ICustError, IError } from 'src/app/common-app/modules/user/dataType/data.interface';
import { HttpService } from 'src/app/common-app/service/httpService.service';
import { IBlade, IBmcUser } from './model';

@Injectable({
    providedIn: 'root'
})

export class UsersService {

    constructor(
        private httpServer: HttpService
    ) {

    }

    public getObjects(): Observable<any> {
        return this.httpServer.get('/redfish/v1/chassisoverview').pipe(
            pluck('body'),
            map((res: { Components: any }) => {
                const bmcUserArr = [];
                const members: any[] = res.Components;
                members.forEach(item => {
                    const tempSlot: IBlade = {};
                    if (item.Id.indexOf('Blade') > -1 && item.ManagedByMM) {
                        tempSlot.id = item.Id.substring(5);
                        tempSlot.label = item.Id;
                    }
                    const swiList = ['Swi1', 'Swi2', 'Swi3', 'Swi4'];
                    if (swiList.indexOf(item.Id) > -1 && item.ManagedByMM) {
                        tempSlot.id = item.Id.substring(3);
                        if (item.Id.indexOf('Swi1') > -1) {
                            tempSlot.label = 'Swi1';
                            tempSlot.id = '33';
                        }
                        if (item.Id.indexOf('Swi2') > -1) {
                            tempSlot.label = 'Swi2';
                            tempSlot.id = '34';
                        }
                        if (item.Id.indexOf('Swi3') > -1) {
                            tempSlot.label = 'Swi3';
                            tempSlot.id = '35';
                        }
                        if (item.Id.indexOf('Swi4') > -1) {
                            tempSlot.label = 'Swi4';
                            tempSlot.id = '36';
                        }
                    }

                    if (Object.keys(tempSlot).length > 0) {
                        tempSlot.list = [];
                        bmcUserArr.push(tempSlot);
                    }
                });
                return bmcUserArr;
            })
        );
    }

    public getSingleObjectUsers(id): Observable<IBmcUser[]> {
        const params = { slot: id };
        return this.httpServer.get('/redfish/v1/AccountService/Oem/Huawei/Nodes', { params }).pipe(
            pluck('body'),
            map((res: { Members: any[] }) => {
                const members = res.Members;
                const list: IBmcUser[] = [];
                members.forEach(item => {
                    const user: IBmcUser = {};
                    user.url = item['@odata.id'];
                    user.id = item.Id;
                    user.userName = item.UserName;
                    user.role = item.RoleId;
                    user.belongTo = '';
                    list.push(user);
                });
                return list;
            })
        );
    }

    // 添加BMC用户
    public addBmcUser(param, password): Observable<any> {
        if (password) {
            param.ReauthKey = encodeURIComponent(password);
        }
        return this.httpServer.post('/redfish/v1/AccountService/Oem/Huawei/Nodes', param, { type: 'redfish' }).pipe(
            catchError((error: HttpErrorResponse) => {
                const errors = this.packingErrorMsg(error.error.error['@Message.ExtendedInfo']);
                throw { type: 'error', errors };
            })
        );
    }

    // 查询Etag
    public queryEtag(url): Observable<any> {
        return this.httpServer.get(url).pipe(
            map((res: HttpResponse<any>) => {
                return res.headers.get('etag') || '';
            })
        );
    }

    // 编辑用户
    public editBmcUser(url: string, param: any, password: string, etag: string): Observable<any> {
        if (password) {
            param.ReauthKey = encodeURIComponent(password);
        }
        return this.httpServer.patch(url, param, { type: 'redfish', headers: { 'If-Match': etag } }).pipe(
            map((res: HttpResponse<any>) => {
                const data = res.body;
                const result: ICustError = { type: 'success', errors: [] };
                let errors: IError[] = [];
                if (data['@Message.ExtendedInfo']) {
                    errors = this.packingErrorMsg(data['@Message.ExtendedInfo']);
                    result.type = 'someFailed';
                    result.errors = errors;
                }
                return result;
            }),
            catchError((error: HttpErrorResponse) => {
                const errors = this.packingErrorMsg(error.error.error['@Message.ExtendedInfo']);
                throw { type: 'error', errors };
            })
        );
    }

    // 删除用户
    public deleteUser(url: string, password: string) {
        if (password) {
            return this.httpServer.delete(url, { type: 'redfish' }, { ReauthKey: encodeURIComponent(password) }).pipe(
                pluck('body')
            );
        } else {
            return this.httpServer.delete(url, { type: 'redfish' }).pipe(
                pluck('body')
            );
        }
    }

    private packingErrorMsg(error: IBackRedfishError[]): IError[] {
        // 封装的错误对象结构为 [{errorId: 'id1', relatedProp: 'Password'}]
        const extendedInfo = error;
        const errorData: IError[] = [];
        extendedInfo.forEach((errorItem) => {
            const errorId = errorItem.MessageId.split('.').pop();
            let relatedProp = '';
            if (errorItem.RelatedProperties && errorItem.RelatedProperties[0]) {
                relatedProp = errorItem.RelatedProperties[0].slice(2);
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
