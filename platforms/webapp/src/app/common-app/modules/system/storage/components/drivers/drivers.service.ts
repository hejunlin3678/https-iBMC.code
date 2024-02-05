import { Injectable } from '@angular/core';
import { HttpService } from 'src/app/common-app/service';
import { map } from 'rxjs/internal/operators/map';
import { isEmptyProperty } from 'src/app/common-app/utils';
import { Observable } from 'rxjs/internal/Observable';
import { of } from 'rxjs/internal/observable/of';
import { forkJoin } from 'rxjs/internal/observable/forkJoin';
import { RaidNode, StorageTree, DriverNode, Driver } from '../../models';
import { getUnitConversionByte } from '../../utils';
import { RaidService } from '../raid/raid.service';
import { TranslateService } from '@ngx-translate/core';

@Injectable({
    providedIn: 'root'
})
export class DriversService {

    constructor(
        private http: HttpService,
        private raidService: RaidService,
        private i18n: TranslateService
    ) { }

    factory(driverNode: DriverNode): Observable<Driver> {
        let raidNode: RaidNode = null;
        let getRaidPost$ = of('').pipe(map(() => null));
        const getDriverPost$ = this.getData(driverNode.url);
        if (driverNode.getIsRaid) {
            raidNode = StorageTree.getInstance().getParentRaid(driverNode.getRaidIndex) as RaidNode;
            if (raidNode.getRaid) {
                getRaidPost$ = of('').pipe(map(() => raidNode.getRaid));
            } else {
                getRaidPost$ = this.raidService.factory(raidNode);
            }
        }
        return forkJoin([getRaidPost$, getDriverPost$]).pipe(
            map(([raidPostData, driverPostData]) => {
                if (raidNode && raidPostData) {
                    raidNode.setRaid = raidPostData;
                }
                return this.createDriver(driverPostData, driverNode);
            })
        );
    }

    private createDriver(bodyData: any, driverNode: DriverNode): Driver {
        const interType = isEmptyProperty(bodyData.Protocol);
        const healthState = bodyData.Health;
        const manufacturer = isEmptyProperty(bodyData.Manufacturer);
        const model = isEmptyProperty(bodyData.Model);
        const serialNumber = isEmptyProperty(bodyData.SerialNumber);
        const firmwareVersion = isEmptyProperty(bodyData.Revision);
        const mediaType = isEmptyProperty(bodyData.MediaType);
        const temperature = isEmptyProperty(bodyData.TemperatureCelsius, ' ℃');
        const firmwareStatus = isEmptyProperty(bodyData.FirmwareStatus);
        const sasAddress0 = isEmptyProperty(bodyData.SASAddress[0]);
        const sasAddress1 = isEmptyProperty(bodyData.SASAddress[1]);
        const capacity = getUnitConversionByte(bodyData.CapacityBytes);
        const capableSpeed = isEmptyProperty(bodyData.CapableSpeedGbs, ' Gbps');
        const negotiatedSpeed = isEmptyProperty(bodyData.NegotiatedSpeedGbs, ' Gbps');
        const powerState = isEmptyProperty(bodyData.PowerState);
        let hotspareType = isEmptyProperty(bodyData.HotspareType);
        if (hotspareType === 'Auto Replace') {
            hotspareType = 'AutoReplace';
        }
        const rebuildState = bodyData.RebuildState;
        let tmpRebuildState = null;
        if (rebuildState === 'DoneOrNotRebuilt') {
            tmpRebuildState = 'FDM_STOPPED';
        } else if (rebuildState === 'Rebuilding') {
            const rebuildProgress = bodyData.RebuildProgress == null ? '' : bodyData.RebuildProgress;
            tmpRebuildState = this.i18n.instant('FDM_RECONSTRUCTION') + '  ' + rebuildProgress;
        }
        const construction = isEmptyProperty(tmpRebuildState);
        const patrolState = isEmptyProperty(bodyData.PatrolState);
        const indicator = isEmptyProperty(bodyData.IndicatorLED);
        const powerSupply = isEmptyProperty(bodyData.HoursOfPoweredUp, ' h');
        const wearRate = isEmptyProperty(bodyData.PredictedMediaLifeLeftPercent, '%');
        const partNum = isEmptyProperty(bodyData.PartNum);
        const resourceOwner = isEmptyProperty(bodyData.AssociatedResource);
        const diskSpeed = isEmptyProperty(bodyData.RotationSpeedRPM);
        const isEPD = Boolean(bodyData.IsEPD);
        const bootEnabled = bodyData.BootEnabled;
        const bootPriority = bodyData.BootPriority || null;
        return new Driver(
            interType,
            healthState,
            manufacturer,
            model,
            serialNumber,
            firmwareVersion,
            mediaType,
            temperature,
            firmwareStatus,
            sasAddress0,
            sasAddress1,
            capacity,
            capableSpeed,
            negotiatedSpeed,
            powerState,
            hotspareType,
            construction,
            patrolState,
            indicator,
            powerSupply,
            wearRate,
            partNum,
            resourceOwner,
            driverNode.getIsRaid,
            diskSpeed,
            isEPD,
            driverNode.getRaidType,
            bootEnabled,
            bootPriority
        );
    }

    private getData(url: string): Observable<any> {
        return this.http.get(url).pipe(
            map(
                (data: any) => data.body
            )
        );
    }
}
