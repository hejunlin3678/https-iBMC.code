import { Injectable } from '@angular/core';
import { HttpService } from 'src/app/common-app/service';
import { getFanInfo } from 'src/app/common-app/utils/fan.util';
import { FanInfo } from 'src/app/common-app/models/fanInfo';
import { pluck } from 'rxjs/internal/operators/pluck';
import { map } from 'rxjs/internal/operators/map';
import { Observable } from 'rxjs/internal/Observable';
import { TranslateService } from '@ngx-translate/core';
@Injectable({
    providedIn: 'root'
})
export class FanInfoService {
    constructor(
        private http: HttpService,
        private translate: TranslateService
    ) { }

    public getThermals(): Observable<FanInfo[]> {
        return this.http.get('/UI/Rest/System/Thermal').pipe(
            pluck('body'),
            map((data: any) => {
                return getFanInfo(data, this.translate);
            })
        );
    }
}
