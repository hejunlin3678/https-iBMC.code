import { Injectable } from '@angular/core';
import { HttpService, UserInfoService, CommonService, LanguageService } from 'src/app/common-app/service';
import { TranslateService } from '@ngx-translate/core';
import { IbmaInfo } from '../model/ibma-info';
import { HttpHeaders } from '@angular/common/http';

@Injectable({
    providedIn: 'root'
})
export class IbmaService {

    constructor(
        private http: HttpService,
        private translate: TranslateService,
        private commonService: CommonService,
        private user: UserInfoService,
        private lang: LanguageService
    ) { }

    public getIBMA() {
        return this.http.get('/UI/Rest/BMCSettings/iBMA');
    }

    public usbStickControl(param) {
        return this.http.post(
            '/UI/Rest/BMCSettings/iBMA/USBStickControl',
            param
        );
    }
    public refreshActivationState() {
        return this.http.post(`/UI/Rest/KeepAlive`, { Mode: 'Activate' }, { ignoreKeepAlive: true });
    }
    public exportKvmStartupFile() {
        return new Promise((resolve, reject) => {
            const param = {
                Mode: 'Shared',
                Language: this.lang.locale.split('-')[0]
            };
            const httpOptions = {};
            const headers = new HttpHeaders({
                'Content-Type': 'application/json;charset=utf-8',
                'Accept': 'application/json'
            });
            httpOptions['headers'] = headers;
            httpOptions['responseType'] = 'blob';
            this.http.post(
                '/UI/Rest/Services/KVM/GenerateStartupFile',
                param,
                httpOptions
            ).subscribe((res) => {
                const data = res['body'];
                this.commonService.downloadFileWithResponse(data, 'kvm.jnlp');
            }, (error) => {
                reject(error);
            });
        });
    }

    public getIBMAInfo(data) {
        const availiableInstallationPackages = data.AvailiableInstallationPackages;
        const installables = [];
        if (availiableInstallationPackages) {
            for (const availiable of availiableInstallationPackages) {
                installables.push(new IbmaInfo(availiable.OSType, availiable.Version));
            }
        }
        const installedPackage = data.InstalledPackage;
        const targetData = {
            installableVersionShow: data.iBMAPacketStatus === 'iBMAPacketIsAvailable',
            ibmaInfos: installables,
            inserted: data.Inserted,
            insertedStr: data.Inserted ? this.translate.instant('IBMA_READY') : this.translate.instant('IBMA_NOTREADY'),
            version: installedPackage.Version,
            driverVersion: installedPackage.DriverVersion,
            runningStatus: installedPackage.RunningStatus
        };
        return targetData;
    }
}
