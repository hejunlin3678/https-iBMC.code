import { TranslateService } from '@ngx-translate/core';
import { Component, OnInit } from '@angular/core';
import { Subscription } from 'rxjs';
import { TiTableRowData, TiTableSrcData, TiTableColumns, TiMessageService } from '@cloud/tiny3';
import { OnlineUserService } from './service';
import { ITableRow } from './domain';
import { AlertMessageService, LoadingService, UserInfoService } from 'src/app/common-app/service';

@Component({
    selector: 'app-online-user',
    templateUrl: './online-user.component.html',
    styleUrls: ['./online-user.component.scss']
})
export class OnlineUserComponent implements OnInit {

    private getSessionSub: Subscription;

    private deleteSessionByIdSub: Subscription;

    public displayed: TiTableRowData[] = [];

    public loaded = false;

    public srcData: TiTableSrcData = {
        data: [],
        state: {
            searched: false,
            sorted: false,
            paginated: false
        }
    };

    public columns: TiTableColumns[] = [
        {
            title: 'ONLINE_NO',
            width: '10%',
            content: '',
        },
        {
            title: 'COMMON_USERNAME',
            width: '20%',
            content: 'name',
        },
        {
            title: 'ONLINE_LOGIN_METHOD',
            width: '20%',
            content: 'login',
        },
        {
            title: 'COMMON_LOGIN_IP',
            width: '20%',
            content: 'ip',
        },
        {
            title: 'COMMON_LOGIN_TIME',
            width: '20%',
            content: 'time',
        },
        {
            title: 'COMMON_OPERATE',
            width: '10%',
            content: 'mySession',
        },
    ];

    private async getSession() {
        this.srcData.data = [];
        this.loaded = false;

        this.getSessionSub = this.onlineServ.findSessions().subscribe((res: any) => {
            res['body'].Members.forEach(item => {
                this.srcData.data.push(
                    {
                        name: item.UserName,
                        login: item.Interface,
                        ip: item.IPAddress,
                        time: item.LoginTime.replace(/[T]/, ' ').slice(0, -6),
                        id: item.SessionID,
                        mySession: item.MySession
                    }
                );
            });
        });

        this.loaded = true;
    }

    public showLogoutMsg(row: ITableRow) {
        const messageInstance = this.tiMessage.open({
            id: 'logoutModel',
            type: 'warn',
            okButton: {
                show: true,
                text: this.translate.instant('COMMON_OK'),
                autofocus: false,
                click: () => {
                    messageInstance.close();
                    this.logout(row);
                }
            },
            cancelButton: {
                show: true,
                text: this.translate.instant('COMMON_CANCEL'),
            },
            title: this.translate.instant('COMMON_CONFIRM'),
            content: this.translate.instant('COMMON_ASK_OK'),
        });
    }

    public logout(row: ITableRow) {
        this.deleteSessionByIdSub = this.onlineServ.deleteSessionById(row.id).subscribe((res) => {
            this.alertServ.eventEmit.emit({
                type: 'success',
                label: 'COMMON_SUCCESS'
            });
            this.getSession();
        }, () => {
            this.alertServ.eventEmit.emit({
                type: 'error',
                label: 'COMMON_FAILED'
            });
        });
    }

    constructor(
        private onlineServ: OnlineUserService,
        private tiMessage: TiMessageService,
        private translate: TranslateService,
        private alertServ: AlertMessageService,
        private loadingServ: LoadingService,
        private userServ: UserInfoService,
    ) { }

    async ngOnInit() {
        this.loadingServ.state.next(true);
        await this.getSession();
        this.loadingServ.state.next(false);
    }

    ngOnDestroy(): void {
        this.getSessionSub.unsubscribe();
        if (this.deleteSessionByIdSub && this.deleteSessionByIdSub.unsubscribe) {
            this.deleteSessionByIdSub.unsubscribe();
        }
    }

}
