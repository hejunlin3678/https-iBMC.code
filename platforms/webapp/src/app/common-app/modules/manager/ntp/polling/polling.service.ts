import { Injectable } from '@angular/core';
import { NtpService } from '../ntp.service';
import { map } from 'rxjs/internal/operators/map';
import { PollingData, Polling, NTPStaticModel } from '../models';
import { TranslateService } from '@ngx-translate/core';
import { UserInfoService } from 'src/app/common-app/service';

@Injectable({
    providedIn: 'root'
})
export class PollingService {

    constructor(
        private i18n: TranslateService,
        private user: UserInfoService
    ) { }

    factory(ntpService: NtpService) {
        return ntpService.data.pipe(
            map((results) => {
                const pollingData = new PollingData();
                const minPollingInterval = results.MinPollingInterval ? results.MinPollingInterval : null;
                const maxPollingInterval = results.MaxPollingInterval ? results.MaxPollingInterval : null;
                const privilege = this.user.hasPrivileges(NTPStaticModel.getInstance().privileges);
                const polling = new Polling(minPollingInterval, maxPollingInterval);
                pollingData.setPollingDisable(privilege);
                if (minPollingInterval) {
                    pollingData.setMinSelect(pollingData.getPollingList[minPollingInterval - 3]);
                }

                if (maxPollingInterval) {
                    pollingData.setMaxSelect(pollingData.getPollingList[maxPollingInterval - 3]);
                }

                return { PollingData: pollingData, Polling: polling };
            })
        );
    }

    public save(pollingCopy: any, polling: Polling): object {
        const obj = {};
        for (const attr in pollingCopy) {
            if (pollingCopy[attr] !== polling[attr]) {
                const key = attr.substring(0, 1).toUpperCase() + attr.substring(1);
                obj[key] = polling[attr];
            }
        }
        return obj;
    }
}
