import { Injectable } from '@angular/core';
import { IEmmInfo, IEmmInfoDetail, UpgradeModel } from '../../model';
import { HttpService, UserInfoService } from 'src/app/common-app/service';
import { HttpErrorResponse, HttpResponse } from '@angular/common/http';
import { Observable } from 'rxjs/internal/Observable';
import { CommonData } from 'src/app/common-app/models';
import { TranslateService } from '@ngx-translate/core';
import { UpgradeService } from '../../services';
@Injectable({
    providedIn: 'root'
})
export class EmmInfoService {
    constructor(
        private http: HttpService,
        private translate: TranslateService,
        private userInfo: UserInfoService,
        private upgradeService: UpgradeService,
        private user: UserInfoService
    ) { }

    public privileges = this.user.hasPrivileges(['ConfigureComponents']);
    public upgrade = new UpgradeModel();
    // 获取emm列表
    public getEmmList(managersid, backupManagersid, standbyPresent): Observable<IEmmInfo[]> {
        return new Observable(observer => {
            const dataList = [
                // 0主板信息
                this.getHmmInfo(managersid),
                this.getHmmChassisInfo(managersid),
                // 2主板CPLD
                this.getHmmCPLD(managersid),
                // 3主板主分区Uboot版本
                this.getHmmActiveUboot(managersid),
                // 4主板备分区Uboot版本
                this.getHmmBackupUboot(managersid),
                // 5主板主分区备份区software版本
                this.getActiveSoftware(managersid),
                // 6主板备分区备份区software版本
                this.getBackupSoftware(managersid),
                // 7主板可用分区software版本
                this.getActiveBackupSoftware(managersid),
                // 8备板信息
                standbyPresent === 1 ? this.getHmmInfo(backupManagersid) : '',
                standbyPresent === 1 ? this.getHmmChassisInfo(backupManagersid) : '',
                // 10备板CPLD
                standbyPresent === 1 ? this.getHmmCPLD(backupManagersid) : '',
                // 11备板主分区Uboot版本
                standbyPresent === 1 ? this.getHmmActiveUboot(backupManagersid) : '',
                // 12备板备分区Uboot版本
                standbyPresent === 1 ? this.getHmmBackupUboot(backupManagersid) : '',
                // 13备板主分区备份区software版本
                standbyPresent === 1 ? this.getActiveSoftware(backupManagersid) : '',
                // 14备板备分区备份区software版本
                standbyPresent === 1 ? this.getBackupSoftware(backupManagersid) : '',
                // 15备板可用分区software版本
                standbyPresent === 1 ? this.getActiveBackupSoftware(backupManagersid) : '',
            ];
            Promise.all(dataList).then((resArr: any[]) => {
                const mainId = managersid?.replace('H', '');
                const standbyId = backupManagersid?.replace('H', '');
                const upgrade = new UpgradeModel();
                upgrade.setMainModel(resArr[1]?.Model);
                const resultArr: IEmmInfo[] = [];
                const oemData1 = this.upgradeService.getOemData(resArr[1]);
                const oemData8 = this.upgradeService.getOemData(resArr[9]);
                // 主用板详情
                const mainInfoDetail = new IEmmInfoDetail(
                    mainId + this.translate.instant('EMM_UPGRADE_MAIN'),
                    resArr[1]?.Model || CommonData.isEmpty,
                    resArr[0]?.FirmwareVersion || CommonData.isEmpty,
                    resArr[2]?.Version || CommonData.isEmpty,
                    resArr[3]?.Version || CommonData.isEmpty,
                    resArr[4]?.Version || CommonData.isEmpty,
                    resArr[5]?.Version || CommonData.isEmpty,
                    resArr[6]?.Version || CommonData.isEmpty,
                    oemData1?.Version?.PCBVersion || CommonData.isEmpty,
                    oemData1?.Version?.BomVersion || CommonData.isEmpty,
                    resArr[7]?.Version || CommonData.isEmpty,
                );
                const mainInfo: IEmmInfo = {
                    id: mainId,
                    name: resArr[1]?.Model || CommonData.isEmpty,
                    softwareVersion: resArr[0]?.FirmwareVersion || CommonData.isEmpty,
                    spare: this.privileges,
                    cpldVersion: resArr[2]?.Version || CommonData.isEmpty,
                    infoDetail: mainInfoDetail,
                    upgrade
                };
                resultArr.push(mainInfo);
                // 备板在位才显示
                if (standbyPresent === 1 || standbyPresent === -1) {
                    // 备用板详情
                    const standbyInfoDetail = new IEmmInfoDetail(
                        standbyId + this.translate.instant('EMM_UPGRADE_STANDBY'),
                        resArr[9]?.Model || CommonData.isEmpty,
                        resArr[8]?.FirmwareVersion || CommonData.isEmpty,
                        resArr[10]?.Version || CommonData.isEmpty,
                        resArr[11]?.Version || CommonData.isEmpty,
                        resArr[12]?.Version || CommonData.isEmpty,
                        resArr[13]?.Version || CommonData.isEmpty,
                        resArr[14]?.Version || CommonData.isEmpty,
                        oemData8?.Version?.PCBVersion || CommonData.isEmpty,
                        oemData8?.Version?.BomVersion || CommonData.isEmpty,
                        resArr[15]?.Version || CommonData.isEmpty,
                    );
                    const standbyInfo: IEmmInfo = {
                        id: standbyId,
                        name: resArr[9]?.Model || CommonData.isEmpty,
                        softwareVersion: resArr[8]?.FirmwareVersion || CommonData.isEmpty,
                        spare: false,
                        cpldVersion: resArr[10]?.Version || CommonData.isEmpty,
                        infoDetail: standbyInfoDetail
                    };
                    resultArr.push(standbyInfo);
                }
                observer.next(resultArr);
            }).catch((error) => {
                observer.error(error);
            });
        });
    }
    public getHmmChassisInfo(managersid) {
        return new Promise((resolve, reject) => {
            this.http.get('/redfish/v1/Chassis/' + managersid).subscribe({
                next: (res: HttpResponse<any>) => {
                    resolve(res.body);
                },
                error: (error: HttpErrorResponse) => {
                    resolve(null);
                }
            });
        });
    }
    public getHmmInfo(managersid) {
        return new Promise((resolve, reject) => {
            this.http.get('/redfish/v1/Managers/' + managersid).subscribe({
                next: (res: HttpResponse<any>) => {
                    resolve(res.body);
                },
                error: (error: HttpErrorResponse) => {
                    resolve(null);
                }
            });
        });
    }
    public getHmmCPLD(managersid) {
        return new Promise((resolve, reject) => {
            this.http.get('/redfish/v1/UpdateService/FirmwareInventory/' + managersid + 'CPLD').subscribe({
                next: (res: HttpResponse<any>) => {
                    resolve(res.body);
                },
                error: (error: HttpErrorResponse) => {
                    resolve(null);
                }
            });
        });
    }
    public getHmmActiveUboot(managersid) {
        return new Promise((resolve, reject) => {
            this.http.get('/redfish/v1/UpdateService/FirmwareInventory/' + managersid + 'ActiveUboot').subscribe({
                next: (res: HttpResponse<any>) => {
                    resolve(res.body);
                },
                error: (error: HttpErrorResponse) => {
                    resolve(null);
                }
            });
        });
    }
    public getHmmBackupUboot(managersid) {
        return new Promise((resolve, reject) => {
            this.http.get('/redfish/v1/UpdateService/FirmwareInventory/' + managersid + 'BackupUboot').subscribe({
                next: (res: HttpResponse<any>) => {
                    resolve(res.body);
                },
                error: (error: HttpErrorResponse) => {
                    resolve(null);
                }
            });
        });
    }
    public getActiveSoftware(managersid) {
        return new Promise((resolve, reject) => {
            this.http.get('/redfish/v1/UpdateService/FirmwareInventory/' + managersid + 'ActiveSoftware').subscribe({
                next: (res: HttpResponse<any>) => {
                    resolve(res.body);
                },
                error: (error: HttpErrorResponse) => {
                    resolve(null);
                }
            });
        });
    }
    public getBackupSoftware(managersid) {
        return new Promise((resolve, reject) => {
            this.http.get('/redfish/v1/UpdateService/FirmwareInventory/' + managersid + 'BackupSoftware').subscribe({
                next: (res: HttpResponse<any>) => {
                    resolve(res.body);
                },
                error: (error: HttpErrorResponse) => {
                    resolve(null);
                }
            });
        });
    }
    public getActiveBackupSoftware(managersid) {
        return new Promise((resolve, reject) => {
            this.http.get('/redfish/v1/UpdateService/FirmwareInventory/' + managersid + 'AvailableBMC').subscribe({
                next: (res: HttpResponse<any>) => {
                    resolve(res.body);
                },
                error: (error: HttpErrorResponse) => {
                    resolve(null);
                }
            });
        });
    }

    // 固件升级重启
    public resetFirmware(hmmId) {
        const header = {
            headers: {
                'Token': this.userInfo.getToken(),
                From: 'WebUI',
                Accept: '*/*'
            }
        };
        return this.http.post('/redfish/v1/Managers/' + hmmId + '/Actions/Manager.Reset',
            {
                ResetType: 'ForceRestart'
            }, header);
    }

    // 判断获取主备板
    public getManagersid() {
        return this.http.get('/UI/Rest/BMCSettings/Failover');
    }
}
