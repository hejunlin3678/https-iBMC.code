import { Injectable } from '@angular/core';
import { HttpService } from 'src/app/common-app/service';
import { ILcdInfo } from '../../model';
import { pluck } from 'rxjs/internal/operators/pluck';
import { map } from 'rxjs/internal/operators/map';
import { Observable } from 'rxjs/internal/Observable';
import { forkJoin } from 'rxjs';
import { CommonData } from 'src/app/common-app/models';
@Injectable({
    providedIn: 'root'
})
export class LCDService {
    constructor(
        private http: HttpService
    ) { }

    factory(): Observable<ILcdInfo[]> {
        const getLcdInfo$ = this.http.get('/redfish/v1/UpdateService/FirmwareInventory/Lcd').pipe(pluck('body'));
        return forkJoin([getLcdInfo$]).pipe(
            map(
                ([data]) => {
                    return this.factoryLcdInfo(data);
                }
            )
        );
    }
    private factoryLcdInfo(data): ILcdInfo[] {
        const resultData: ILcdInfo[] = [];
        if (data?.Status?.State !== 'Absent' && data?.Name) {
            const lcdInfo: ILcdInfo = {
                name: data?.Name || CommonData.isEmpty,
                version: data?.Version || CommonData.isEmpty
            };
            resultData.push(lcdInfo);
        }
        return resultData;
    }
}
