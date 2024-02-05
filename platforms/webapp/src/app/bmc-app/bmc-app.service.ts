import { Injectable } from '@angular/core';
import { HttpService } from 'src/app/common-app/service/httpService.service';
import { Observable } from 'rxjs/internal/Observable';
import { pluck } from 'rxjs/internal/operators/pluck';
import { map } from 'rxjs/internal/operators/map';
import { catchError } from 'rxjs/internal/operators/catchError';
import { of } from 'rxjs/internal/observable/of';
import { WebStyle } from '../common-app/models/common.interface';
import { GlobalData } from '../common-app/models';

@Injectable({
    providedIn: 'root'
})
export class BMCAppService {

    constructor(
        private http: HttpService
    ) { }

    public getStyle(): Observable<string> {
        return this.http.get('/extern/custom/style_config.json?t=' + new Date().getTime()).pipe(
            pluck('body'),
            map((res: any) => {
                const webStyle: string = String(res.pageStyle);
                if (res.pageStyle === WebStyle.StandardShenma) {
                    // 新增神码标准风格适配处理
                    GlobalData.getInstance().setShenmaStyle = true;
                    return WebStyle.Standard;
                } else {
                    for (const key in WebStyle) {
                        if (Object.prototype.hasOwnProperty.call(WebStyle, key) && webStyle === WebStyle[key]) {
                            return webStyle;
                        }
                    }
                    return WebStyle.Default;
                }
            }),
            catchError(
                () => {
                    return of(WebStyle.Default);
                }
            )
        );
    }
}
