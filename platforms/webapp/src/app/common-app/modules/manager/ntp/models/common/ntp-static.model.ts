import { INtpModal, IRadio } from './ntp.interface';
import { TiValidationConfig } from '@cloud/tiny3';
import { TranslateService } from '@ngx-translate/core';

export class NTPStaticModel {

    private constructor() {}
    private static instance: NTPStaticModel;
    private i18n: TranslateService;
    public readonly privileges: string[] = ['ConfigureComponents'];
    private ntpModel: INtpModal = {
        server: {
            id: 'serverPreferred',
            label: 'NTP_SERVER',
            value: null,
            count: 3,
            tip: null
        },
        serverPreferred: {
            id: 'serverPreferred',
            label: 'NTP_SERVER_PREFERRED',
            value: null,
            count: 3,
            tip: null
        },
        serverAlternate: {
            id: 'serverAlternate',
            label: 'NTP_SERVER_ALTERNATE',
            value: null,
            count: 3,
            tip: null
        }
    };

    public readonly radioList: IRadio[] = [
        {
            key: 'NTP_DHCPV4_AUTOGET',
            id: 'IPv4',
            disable: false
        }, {
            key: 'NTP_DHCPV6_AUTOGET',
            id: 'IPv6',
            disable: false
        }, {
            key: 'NTP_MANUAL_CONFIGURATION',
            id: 'Static',
            disable: false
        }
    ];

    set setI18n(i18n: TranslateService) {
        this.i18n = i18n;
        this.ntpModel.server.tip = {
            tip: this.i18n.instant('COMMON_IPV4_IPV6_DOMIN'),
            type: 'blur',
            tipPosition: 'right',
            errorMessage: {
                pattern: this.i18n.instant('COMMON_FORMAT_ERROR')
            }
        };
        const topNTPTip: TiValidationConfig = {
            tip: this.i18n.instant('COMMON_IPV4_IPV6_DOMIN'),
            type: 'blur',
            tipPosition: 'top',
            errorMessage: {
                pattern: this.i18n.instant('COMMON_FORMAT_ERROR')
            }
        };
        this.ntpModel.serverPreferred.tip = topNTPTip;
        this.ntpModel.serverAlternate.tip = topNTPTip;
    }

    get getNTPModel(): INtpModal {
        return this.ntpModel;
    }

    public static getInstance(i18n ?: TranslateService): NTPStaticModel {
        if (!NTPStaticModel.instance) {
            NTPStaticModel.instance = new NTPStaticModel();
        }
        if (i18n) {
            NTPStaticModel.instance.setI18n = i18n;
        }
        return NTPStaticModel.instance;
    }
}



