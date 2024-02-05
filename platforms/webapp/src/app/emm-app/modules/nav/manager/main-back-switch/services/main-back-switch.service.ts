import { Injectable } from '@angular/core';
import { HttpService } from 'src/app/common-app/service';
import { Observable } from 'rxjs/internal/Observable';
import { map, pluck } from 'rxjs/operators';

@Injectable({
    providedIn: 'root'
})

export class MainBackSwitchService {
    constructor (
        private http: HttpService,
    ) { }

    public getMainBackInfo() {
        return this.getBoardInfo();
    }
    private getBoardInfo(): Observable<any> {
        const url = '/UI/Rest/BMCSettings/Failover';
        return this.http.get(url).pipe(
            pluck('body'),
            map((data) => this.initBoardInfo(data))
        );
    }
    // 交换配置
    public switchoverBoard () {
        return this.http.post(`/UI/Rest/BMCSettings/Failover/ForceFailover`, {});
    }

    private initBoardInfo(data) {
        const info = data;
        return info;
    }
}

