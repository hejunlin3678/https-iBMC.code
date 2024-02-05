import { Injectable } from '@angular/core';
import { HttpService, UserInfoService } from 'src/app/common-app/service';
import { TranslateService } from '@ngx-translate/core';
import { UpgradeModel, UpgradeManual, UpgradeVersion } from '../model';
import { map } from 'rxjs/operators';

@Injectable({
    providedIn: 'root'
})
export class UpgradeService {

    constructor(
        private http: HttpService,
        private translate: TranslateService,
        private user: UserInfoService
    ) {}
    public upgrade = new UpgradeModel();
    public upgradeManual = new UpgradeManual();

    public stateChangeCOfm = () => {
        this.upgrade.setFirmFlag(false);
        this.upgrade.setFirmBtnRest(false);
        this.upgrade.setFirmBtnBack(false);
        this.upgrade.setFileBtn(true);
    }

    public getUpgradeInfo = () => {
        return this.http.get('/UI/Rest/BMCSettings/UpdateService').pipe(
            map(response => {
                const tempArrData = response['body'].FirmwareVersion;
                const upgradeVersion = new UpgradeVersion();
                upgradeVersion.setTaskMembers(response['body'].UpdatingFlag);
                upgradeVersion.setActiveBMC(tempArrData.ActiveBMCVersion);
                upgradeVersion.setBackupBMC(tempArrData.BackupBMCVersion);
                // 1711特性
                if (tempArrData.AvailableBMCVersion) {
                    upgradeVersion.setAvailableBMC(tempArrData.AvailableBMCVersion);
                    upgradeVersion.setUnavailable(true);
                } else {
                    upgradeVersion.setAvailableBMC('');
                    upgradeVersion.setUnavailable(false);
                }
                // 版本列表数据组装
                const versionList = [];
                const versionData = response['body'].UpgradeResource;
                let versions = {};
                for (const item of versionData) {
                    if (item.Version && item.Version !== '000') {
                        versions = {
                            name: item.Name,
                            currentVersion: item.Version || '--',
                            type: item.Type
                        };
                        versionList.push(versions);
                    }
                }

                const smartUpgradeSupport = response['body'].SmartUpgradeSupport;
                const queueUpdatingFlag = response['body'].QueueUpdatingFlag;
                // 升级状态
                this.upgradeManual.setTaskMembers(response['body'].UpdatingFlag);

                return {
                    upgradeManual: this.upgradeManual,
                    upgradeVersion,
                    versionList,
                    smartUpgradeSupport,
                    queueUpdatingFlag
                };
            })
        );
    }
}
