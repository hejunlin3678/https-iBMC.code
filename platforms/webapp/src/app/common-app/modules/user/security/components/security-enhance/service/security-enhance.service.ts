import { Injectable } from '@angular/core';
import { HttpService } from 'src/app/common-app/service';
import { Observable } from 'rxjs/internal/Observable';
import { IFormOption, IAuthorityEnum, IAuthorityData, IAccount, SSHAuthenticationMode } from '../security-enhance.dataype';
import { pluck } from 'rxjs/internal/operators/pluck';
import { map } from 'rxjs/internal/operators/map';

@Injectable({
    providedIn: 'root'
})
export class SecurityEnhanceService {

    constructor(
        private http: HttpService
    ) { }


    public getData(): Observable<IAuthorityData> {
        return this.getSecurityInfo();
    }

    private factoryEnhanceData(securityInfo): IAuthorityData {
        const {
            SystemLockDownEnabled: systemLockDownEnabled,
            OSUserManagementEnabled,
            PasswordComplexityCheckEnabled: passwordComplexityCheckEnabled,
            SSHAuthenticationMode: sshPasswordAuthenticationEnabled,
            PasswordValidityDays: passwordValidityDays,
            EmergencyLoginUser: emergencyLoginUser,
            PreviousPasswordsDisallowedCount: previousPasswordsDisallowedCount,
            MinimumPasswordAgeDays: minimumPasswordAgeDays,
            MinimumPasswordLength: minimumPasswordLength,
            AccountInactiveTimelimitDays: accountInactiveTimelimit,
            CertificateOverdueWarningTime: certificateOverdueWarningTime,
            TLSVersion: tlsVersion,
            'TLS1.2Configurable': tls12Configurable,
            AccountLockoutThreshold: accountLockoutThreshold,
            AccountLockoutDuration: accountLockoutDuration,
            AvailableUsers: availableUsers,
            AntiDNSRebindEnabled: antiDNSRebindEnabled
        } = securityInfo;

        const userArr: IAccount[] = [{
            id: '',
            label: '[NULL]'
        }];

        if (availableUsers && availableUsers.length) {
            availableUsers.forEach((userItem) => {
                userArr.push({
                    id: userItem,
                    label: userItem
                });
            });
        }

        const result: IAuthorityData = {
            user: userArr,
            systemLockDownEnabled,
            OSUserManagementEnabled,
            passwordComplexityCheckEnabled,
            tls12Configurable,
            sshPasswordAuthenticationEnabled: sshPasswordAuthenticationEnabled === SSHAuthenticationMode.password,
            passwordValidityDays: passwordValidityDays || 0,
            emergencyLoginUser,
            previousPasswordsDisallowedCount: previousPasswordsDisallowedCount || 0,
            minimumPasswordAgeDays: minimumPasswordAgeDays || 0,
            minimumPasswordLength,
            accountInactiveTimelimit,
            certificateOverdueWarningTime,
            tlsVersion,
            accountLockoutThreshold,
            accountLockoutDuration,
            antiDNSRebindEnabled
        };
        if (securityInfo.error) {
            result.error = securityInfo.error;
        }
        return result;
    }

    private getSecurityInfo(): Observable<any> {
        const url = '/UI/Rest/AccessMgnt/AdvancedSecurity';
        return this.http.get(url).pipe(
            pluck('body'),
            map((securityInfo) => {
                return this.factoryEnhanceData(this.formatSecInfo(securityInfo));
            })
        );
    }

    public saveParams(forms: IFormOption[]) {
        const params: any = {};
        const tlsArr = [];
        for (const form of forms) {
            if (form.associated === IAuthorityEnum.accountLockoutThreshold) {
                params[form.associated] = this.transformToNumber(form.value.id);
                continue;
            }
            if (form.associated === IAuthorityEnum.accountLockoutDuration) {
                params[form.associated] = this.transformToNumber(form.controll.value);
                continue;
            }
            if (form.associated === IAuthorityEnum.tlsVersion && form.value) {
                if (form.value.id === 'TLS1.2') {
                    tlsArr.push('TLS1.2', 'TLS1.3');
                } else {
                    tlsArr.push(form.value.id);
                }
                continue;
            }
            if (form.associated === IAuthorityEnum.SSHPasswordAuthenticationEnabled) {
                params[form.associated] = form.value ? SSHAuthenticationMode.password : SSHAuthenticationMode.publicKey;
                continue;
            }

            if (form.controll) {
                params[form.associated] = this.transformToNumber(form.controll.value);

                continue;
            }
            if (typeof form.value === 'object') {
                params[form.associated] = form.value.id;
                continue;
            }
            params[form.associated] = this.transformToNumber(form.value);
        }

        if (tlsArr.length) {
            params.TLSVersion = tlsArr;
        }

        return this.updateUser(params);
    }

    private updateUser(params: any) {
        const url = '/UI/Rest/AccessMgnt/AdvancedSecurity';
        return this.http.patch(url, params).pipe(
            pluck('body'),
            map((securityInfo) => {
                return this.factoryEnhanceData(this.formatSecInfo(securityInfo));
            })
        );
    }

    private formatSecInfo(securityInfo) {
        if (securityInfo.error) {
            const tempSecInfo: any = Object.assign({}, securityInfo.data);
            tempSecInfo.error = securityInfo['error'];
            return tempSecInfo;
        }
        return securityInfo;
    }

    private transformToNumber(str: string) {
        const transformed = parseInt(str, 10);
        if (isNaN(transformed)) {
            return str;
        }
        return transformed;
    }

}
