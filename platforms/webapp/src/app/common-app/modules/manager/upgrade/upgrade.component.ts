import { Component, OnInit } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import { UserInfoService } from 'src/app/common-app/service';
import { UpgradeService } from './services';
import { LoadingService } from 'src/app/common-app/service';
import { UpgradeVersionService } from './component/upgrade-version/services';

@Component({
    selector: 'app-upgrade',
    templateUrl: './upgrade.component.html',
    styleUrls: ['../manager.component.scss', './upgrade.component.scss']
})
export class UpgradeComponent implements OnInit {

    constructor(
        private translate: TranslateService,
        private user: UserInfoService,
        private upgradeService: UpgradeService,
        private upgradeVersionService: UpgradeVersionService,
        private loadingService: LoadingService
    ) { }

    // 权限判断
    public isSystemLock = this.user.systemLockDownEnabled;

    public upgrade = this.upgradeService.upgrade;
    public upgradeManual = this.upgradeService.upgradeManual;
    public smartUpgradeSupport: boolean = true;
    public updateServiceInfo;
    public upgradeVersion;
    public versionList;

    public tabs = [
        {
            id: 'upgradeVersion',
            title: this.translate.instant('IBMC_UPGRADE_VERSION'),
            active: true,
            hide: true
        },
        {
            id: 'upgradeManual',
            title: this.translate.instant('IBMC_UPGRADE_MANUAL'),
            active: false,
            hide: true
        }
    ];

    public initData = () => {
        // 初始化变量
        this.upgrade.setConfigFinish(false);
        this.upgrade.setTipMessage('');
        this.upgrade.setUpgradeResult('');
        this.upgradeService.stateChangeCOfm();
        this.upgradeManual.setTaskPercentage(0);
        this.upgradeService.getUpgradeInfo().subscribe((res) => {
            this.updateServiceInfo = res.upgradeManual;
            this.upgradeVersion = res.upgradeVersion;
            this.versionList = res.versionList;
            // 根据接口返回是否显示自动更新
            this.smartUpgradeSupport = res.smartUpgradeSupport;
            this.upgradeManual.setQueueUpdatingFlag(res.queueUpdatingFlag);

            for (const item of this.tabs) {
                if (item.id !== 'upgradeVersion' && item.id !== 'upgradeManual') {
                    item.hide = !this.smartUpgradeSupport;
                } else {
                    item.hide = false;
                }
            }
        });
    }
    public lockStateChange(state) {
        if (this.isSystemLock !== state) {
            this.isSystemLock = state;
        }
    }
    ngOnInit(): void {
        this.loadingService.state.next(true);
        this.initData();
    }

}
