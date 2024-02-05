import { Injectable } from '@angular/core';
import { Observable } from 'rxjs/internal/Observable';
import { pluck } from 'rxjs/internal/operators/pluck';
import { HttpService } from './common-app/service';
import { map } from 'rxjs/internal/operators/map';
@Injectable({
  providedIn: 'root'
})
export class AppService {
    constructor(
        private http: HttpService
    ) { }
    private getV1Info(): Observable<any> {
        return this.http.get('/UI/Rest/Login').pipe(
            pluck('body')
        );
    }
    public getInitInfo(): Observable<{ v1Info: any }> {
        return this.getV1Info().pipe(
            map((res) => {
                return { v1Info: res };
            })
        );
    }
}
