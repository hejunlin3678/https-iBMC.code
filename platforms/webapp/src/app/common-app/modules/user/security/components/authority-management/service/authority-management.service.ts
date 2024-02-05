import { Injectable } from '@angular/core';
import { Subject, Observable } from 'rxjs';
import { pluck } from 'rxjs/internal/operators/pluck';
import { map } from 'rxjs/internal/operators/map';
import { ITableRow, IUpdateParam, RoleName } from '../authority-management.datatype';
import { HttpService } from 'src/app/common-app/service';

@Injectable({
    providedIn: 'root'
})
export class AuthorityManagementService {

    private erroMsgSubject = new Subject();

    private password: string;

    public setPwd(str: string) {
        this.password = str;
    }

    public getPwd(): string {
        return this.password;
    }

    public getErroMsg(): Observable<any> {
        return this.erroMsgSubject.asObservable();
    }

    public sendErroMsg(type: any) {
        this.erroMsgSubject.next(type);
    }

    private getRole() {
        const url = '/UI/Rest/AccessMgnt/RightManagement';
        return this.http.get(url).pipe(
            pluck('body'),
            map(rightsData => this.factoryRights(rightsData))
        );
    }

    private factoryRights(rightsData): ITableRow[] {
        const members = rightsData.Members;
        const transformedData: ITableRow[] = [];
        if (members && members.length) {
            members.forEach(member => {
                const {
                    ID: id,
                    Name: name,
                    UserConfigEnabled: userConfigEnabled,
                    BasicConfigEnabled: basicConfigEnabled,
                    RemoteControlEnabled: remoteControlEnabled,
                    VMMEnabled,
                    SecurityConfigEnabled: securityConfigEnabled,
                    PowerControlEnabled: powerControlEnabled,
                    DiagnosisEnabled: diagnosisEnabled,
                    QueryEnabled: queryEnabled,
                    ConfigureSelfEnabled: configureSelfEnabled,
                } = member;

                const { roleName, isOperation, sequence } = this.getRoleName(name);

                if (roleName === 'COMMON_ROLE_NOACCESS') {
                    return;
                }

                transformedData.push({
                    id,
                    isOperation,
                    roleName,
                    userConfigEnabled,
                    basicConfigEnabled,
                    remoteControlEnabled,
                    VMMEnabled,
                    securityConfigEnabled,
                    powerControlEnabled,
                    diagnosisEnabled,
                    queryEnabled,
                    configureSelfEnabled,
                    isSave: false,
                    isEdit: false,
                    sequence,
                });
            });

        }

        return transformedData;
    }

    public getData(): Observable<ITableRow[]> {
        return this.getRole();
    }

    private getParam(row: ITableRow): IUpdateParam {
        return {
            ID: row.id,
            UserConfigEnabled: row.userConfigEnabled,
            BasicConfigEnabled: row.basicConfigEnabled,
            RemoteControlEnabled: row.remoteControlEnabled,
            VMMEnabled: row.VMMEnabled,
            SecurityConfigEnabled: row.securityConfigEnabled,
            PowerControlEnabled: row.powerControlEnabled,
            DiagnosisEnabled: row.diagnosisEnabled,
            ConfigureSelfEnabled: row.configureSelfEnabled,
            ReauthKey: ''
        };
    }

    public saveParWithPwd(row: ITableRow, pwd: string) {
        const param = this.getParam(row);
        return this.roleUpdate(param, row.id, pwd);
    }

    public savePar(row: ITableRow) {
        const param = this.getParam(row);
        return this.roleUpdate(param, row.id);
    }

    private roleUpdate(param: IUpdateParam, id: number, password?: string): Observable<ITableRow> {
        const url = '/UI/Rest/AccessMgnt/RightManagement';
        let roleUpdate$: Observable<any>;
        if (password !== undefined && password !== '') {
            param.ReauthKey = window.btoa(password);
        } else {
            delete param.ReauthKey;
        }
        roleUpdate$ = this.http.patch(url, param);

        return roleUpdate$.pipe(
            pluck('body'),
            map(rightsData => this.factoryRights(rightsData)),
            map(items => this.factoryCurrentRow(items, id)),
        );
    }

    private factoryCurrentRow(items: ITableRow[], id: number) {
        return items.find(item => item.id === id);
    }

    private getRoleName(id: string) {
        let roleName = '';
        let isOperation = false;
        let sequence = 1;
        switch (id) {
            case RoleName.administrator:
                roleName = 'COMMON_ROLE_ADMINISTRATOR';
                sequence = 1;
                break;
            case RoleName.operator:
                roleName = 'COMMON_ROLE_OPERATOR';
                sequence = 2;
                break;
            case RoleName.commonuser:
                roleName = 'COMMON_ROLE_COMMONUSER';
                sequence = 3;
                break;
            case RoleName.noAccess:
                roleName = 'COMMON_ROLE_NOACCESS';
                break;
            case RoleName.customRole1:
                roleName = 'COMMON_ROLE_CUSTOMROLE1';
                isOperation = true;
                sequence = 4;
                break;
            case RoleName.customRole2:
                roleName = 'COMMON_ROLE_CUSTOMROLE2';
                isOperation = true;
                sequence = 5;
                break;
            case RoleName.customRole3:
                roleName = 'COMMON_ROLE_CUSTOMROLE3';
                isOperation = true;
                sequence = 6;
                break;
            case RoleName.customRole4:
                roleName = 'COMMON_ROLE_CUSTOMROLE4';
                isOperation = true;
                sequence = 7;
                break;
        }
        return { roleName, isOperation, sequence };
    }

    constructor(private http: HttpService) { }

}
