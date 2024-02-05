import { Injectable } from '@angular/core';
import { HttpService } from 'src/app/common-app/service/httpService.service';
import { Observable } from 'rxjs/internal/Observable';
import { pluck } from 'rxjs/internal/operators/pluck';
import { map } from 'rxjs/internal/operators/map';
import { forkJoin } from 'rxjs/internal/observable/forkJoin';
import { catchError } from 'rxjs/internal/operators/catchError';
import { of } from 'rxjs/internal/observable/of';
import { WebStyle } from '../common-app/models/common.interface';

@Injectable({
    providedIn: 'root'
})
export class IRMAppService {

    constructor(
        private http: HttpService
    ) { }

    private getV1Info(): Observable<any> {
        return this.http.get('/UI/Rest/Login').pipe(
            pluck('body')
        );
    }

    public getInitInfo(): Observable<{ v1Info: any }> {
        const get$ = [this.getV1Info()];
        return forkJoin(get$).pipe(
            map((res) => {
                return { v1Info: res[0] };
            })
        );
    }
}
