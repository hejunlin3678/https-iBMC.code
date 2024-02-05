import { Injectable } from '@angular/core';
import { NtpService } from '../ntp.service';
import { map } from 'rxjs/internal/operators/map';
import { NTPFunction, NTPFunctionData, INtp } from '../models';
import { NTPStaticModel } from '../models/common/ntp-static.model';
import { NTPFormControl } from '../models/ntp-function/form.model';
import { Validators } from '@angular/forms';
import { SerAddrValidators } from 'src/app/common-app/utils/valid';
import { TranslateService } from '@ngx-translate/core';
import { UserInfoService } from 'src/app/common-app/service';

@Injectable({
    providedIn: 'root'
})
export class NtpFunctionService {

    constructor(
        private i18n: TranslateService,
        private ntpService: NtpService,
        private user: UserInfoService
    ) { }

    public factory() {
        return this.ntpService.data.pipe(
            map((ntpServiceData) => {
                const ntpSwitch = ntpServiceData.Enabled;
                const ntpCount = ntpServiceData.ServerCount || 3;
                let ntpControl = null;
                let ntpControlAlt = null;
                if (ntpCount === 3) {
                    const ntpValue = [];
                    ntpValue.push(ntpServiceData.PreferredServer || '');
                    ntpValue.push(ntpServiceData.AlternateServer || '');
                    ntpValue.push(ntpServiceData.ExtraServer || '');
                    ntpControl = this.ntpCtrlFactory(NTPStaticModel.getInstance(this.i18n).getNTPModel.server, ntpValue);
                } else {
                    const arr = ntpServiceData.ExtraServer.split(',');
                    // 主选服务器IP数组
                    const ntpValuePre = [];
                    // 备选服务器IP数组
                    const ntpValueAlt = [];
                    ntpValuePre.push(ntpServiceData.PreferredServer || '');
                    ntpValuePre.push(ntpServiceData.AlternateServer || '');
                    if (ntpServiceData.ExtraServer.indexOf(',') > -1) {
                        ntpValuePre.push(arr[0] || '');
                        ntpValueAlt.push(arr[1] || '');
                        ntpValueAlt.push(arr[2] || '');
                        ntpValueAlt.push(arr[3] || '');
                    } else {
                        ntpValuePre.push(ntpServiceData.ExtraServer || '');
                        ntpValueAlt.push('', '', '');
                    }
                    ntpControl = this.ntpCtrlFactory(NTPStaticModel.getInstance(this.i18n).getNTPModel.serverPreferred, ntpValuePre);
                    ntpControlAlt = this.ntpCtrlFactory(NTPStaticModel.getInstance(this.i18n).getNTPModel.serverAlternate, ntpValueAlt);
                }
                const privilege = this.user.hasPrivileges(NTPStaticModel.getInstance().privileges);
                const systemLocked = this.user.systemLockDownEnabled || false;
                const ntpData = new NTPFunctionData(
                    {
                        ntpSwitch,
                        ntpCount,
                        ntpControl,
                        ntpControlAlt,
                        privilege,
                        systemLocked
                    }
                );
                ntpData.setRaioDisable(!ntpServiceData.DHCPv4Enable, 0);
                ntpData.setRaioDisable(!ntpServiceData.DHCPv6Enable, 1);
                ntpData.setRaioDisable(false, 2);

                const radioSelect = ntpData.getRadioList.filter((radio) => radio.id === ntpServiceData.AddressMode);
                ntpData.setRadioSelect = radioSelect[0];

                const ntpFunction = new NTPFunction(
                    ntpServiceData.Enabled || false,
                    ntpData.getRadioSelect.id || 'Static',
                    ntpServiceData.PreferredServer || '',
                    ntpServiceData.AlternateServer || '',
                    ntpServiceData.ExtraServer || '',
                );
                return { NTPFunctionData: ntpData, NTPFunction: ntpFunction };
            })
        );
    }

    public save(ntpCopy: any, ntpFunction: NTPFunction): object {
        const obj = {};
        for (const attr in ntpFunction) {
            if (ntpCopy[attr] !== ntpFunction[attr]) {
                const key = attr.substring(0, 1).toUpperCase() + attr.substring(1);
                obj[key] = ntpFunction[attr];
            }
        }
        return obj;
    }

    public setVal(ntpFunctionData: NTPFunctionData): { preferredNtpServer, alternateNtpServer, extraNtpServer } {
        const preferredNtpServer = ntpFunctionData.getNTPServes.controls.serverPreferred0.value;
        const alternateNtpServer = ntpFunctionData.getNTPServes.controls.serverPreferred1.value;
        let extraNtpServer = null;

        if (ntpFunctionData.getNTPCount === 3) {
            extraNtpServer = ntpFunctionData.getNTPServes.controls.serverPreferred2.value;
        } else {
            extraNtpServer =
                ntpFunctionData.getNTPServes.controls.serverPreferred2.value + ',' +
                ntpFunctionData.getNTPServesAlter.controls.serverAlternate0.value + ',' +
                ntpFunctionData.getNTPServesAlter.controls.serverAlternate1.value + ',' +
                ntpFunctionData.getNTPServesAlter.controls.serverAlternate2.value;
        }
        return {
            preferredNtpServer,
            alternateNtpServer,
            extraNtpServer
        };
    }

    /**
     * NTP的FormControl批量生成方法
     * @param ntpModal 模型
     * @param ntpValue 数值
     */
    private ntpCtrlFactory(ntpModal: INtp, ntpValue: string[]) {
        const ntpCtrl = {};
        for (let i = 0; i < ntpModal.count; i++) {
            const id = ntpModal.id + i;
            const label = ntpModal.label + i;
            ntpCtrl[id] = new NTPFormControl(
                label,
                id,
                ntpValue[i],
                Validators.compose([
                    SerAddrValidators.validateIp46AndDomin()
                ]),
                ntpModal.tip
            );
        }
        return ntpCtrl;
    }
}
