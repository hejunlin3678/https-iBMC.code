import { Injectable } from '@angular/core';
import { HttpService } from 'src/app/common-app/service/httpService.service';
import { Observable } from 'rxjs/internal/Observable';

@Injectable({
    providedIn: 'root'
})
export class BaseInfoService {

    constructor(
        private http: HttpService
    ) { }

    public setUIDState(param): Observable<any> {
        return new Observable((observe) => {
            this.http.patch('/UI/Rest/GenericInfo', param).subscribe({
                next: (res) => {
                    observe.next(param.IndicatorLEDState);
                },
                error: (error) => {
                    observe.error(error);
                }
            });
        });
    }

    public setPowerState(param: { type: string }): Observable<any> {
        return new Observable((observe) => {
            this.http.post('/UI/Rest/System/PowerControl', {
                OperateType: param.type
            }).subscribe({
                next: (res) => {
                    observe.next(param.type);
                },
                error: (error) => {
                    observe.error(error);
                }
            });
        });
    }
}
