import { Component, OnInit, Output, EventEmitter } from '@angular/core';
import { Router } from '@angular/router';
import { UserInfoService } from 'src/app/common-app/service/user-info.service';
import { GlobalDataService } from 'src/app/common-app/service/global-data.service';

@Component({
    selector: 'app-lock-alert',
    templateUrl: './lock-alert.component.html',
    styleUrls: ['./lock-alert.component.scss']
})
export class LockAlertComponent implements OnInit {
    @Output() stateChange: EventEmitter<boolean> = new EventEmitter();

    constructor(
        private router: Router,
        private userInfo: UserInfoService,
        private globalData: GlobalDataService
    ) { }
    public type: string = 'warn';
    public triggerScroll: boolean = true;
    public typeIcon: boolean = true;
    public closeIcon: boolean = false;
    public systemLockState: boolean = this.userInfo.systemLockDownEnabled;

    ngOnInit() {
        this.stateChange.emit(this.userInfo.systemLockDownEnabled);
        this.globalData.systemLockState.subscribe((state: boolean) => {
            this.stateChange.emit(state);
            this.systemLockState = state;
        });
    }

    public goToSecurityConfiguration() {
        this.router.navigate(['/navigate/user/security']);
    }
}
