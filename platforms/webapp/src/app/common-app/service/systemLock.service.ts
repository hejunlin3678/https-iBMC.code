import { Injectable } from '@angular/core';
import { Subject } from 'rxjs';
import { UserInfoService } from './user-info.service';
import { GlobalDataService } from './global-data.service';

@Injectable({
    providedIn: 'root'
})
export class SystemLockService {

    constructor(
        private userInfo: UserInfoService,
        private globalData: GlobalDataService
    ) {
        // 无法初始化时，开始监听系统锁定状态
        this.globalData.systemLockState.subscribe({
            next: state => {
                this.lockStateChange.next(state);
            }
        });
    }

    public lockState: boolean;
    public lockStateChange: Subject<boolean> = new Subject();

    // 获取当前的系统锁定状态
    public getState(): boolean {
        this.lockState = this.userInfo.systemLockDownEnabled;
        return this.lockState;
    }
}
