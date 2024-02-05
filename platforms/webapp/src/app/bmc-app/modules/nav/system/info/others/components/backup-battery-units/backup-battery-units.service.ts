import { Injectable } from '@angular/core';
import { BackupBatteryUnitsArray, BackupBatteryUnits } from '../../models';
import { Observable } from 'rxjs/internal/Observable';
import { of } from 'rxjs/internal/observable/of';
import { map } from 'rxjs/internal/operators/map';
import { forkJoin } from 'rxjs/internal/observable/forkJoin';
import { OthersService } from '../../others.service';
import { CardUrlArray } from '../../models/cardUrlArray';
import { isOtherEmptyProperty } from 'src/app/common-app/utils';

@Injectable({
    providedIn: 'root'
})
export class BackupBatteryUnitsService {

    constructor(
        private othersService: OthersService
    ) { }

    factory(activeCard: CardUrlArray): Observable<BackupBatteryUnitsArray> {
        const backupBatteryUnitsArray = new BackupBatteryUnitsArray();
        // forkJoin必须拥有两个流，先放两个空流进去防止报错
        const post$ = [of('').pipe(map(() => null)), of('').pipe(map(() => null))];
        if (activeCard.getCardUrlArray && activeCard.getTotalCount) {
            activeCard.getCardUrlArray.forEach((url) => {
                post$.push(this.othersService.getData(url).pipe(map((bodyDatas) => {
                    bodyDatas.forEach((bodyData) => {
                    const boardName = isOtherEmptyProperty(bodyData.BoardName) || ' ';
                    const id = bodyData.Id;
                    const firmwareVersion = isOtherEmptyProperty(bodyData.FirmwareVersion) || ' ';
                    const workingState = isOtherEmptyProperty(bodyData.WorkingState) || 0;
                    const capacity = isOtherEmptyProperty(bodyData.Battery.RemainingPowerMilliWatts) || ' ';
                    const model = isOtherEmptyProperty(bodyData.Battery.Model) || ' ';
                    const serialNumber = isOtherEmptyProperty(bodyData.Battery.SerialNumber) || ' ';
                    const manufacturer = isOtherEmptyProperty(bodyData.Battery.Manufacturer) || ' ';
                    const presence = (bodyData.M2Device1Presence ? 'Presence' : 'Absence') +
                        '/' +
                        (bodyData.M2Device2Presence ? 'Presence' : 'Absence');
                    backupBatteryUnitsArray.addBackupBatteryUnits(
                        new BackupBatteryUnits(
                            boardName,
                            id,
                            firmwareVersion,
                            workingState,
                            capacity,
                            model,
                            serialNumber,
                            manufacturer,
                            presence
                        ));
                    });
                })));
            });
        }
        return forkJoin(post$).pipe(
            map(() => backupBatteryUnitsArray)
        );
    }

}
