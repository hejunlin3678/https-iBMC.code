import { Injectable } from '@angular/core';
import { HttpService, UserInfoService } from 'src/app/common-app/service';
import { Observable } from 'rxjs/internal/Observable';

@Injectable({
    providedIn: 'root'
})
export class LanguageManageService {

    constructor(
        private http: HttpService,
        private user: UserInfoService
    ) {

    }

    public getLangInfo(): Observable<any> {
        return this.http.get(`/UI/Rest/BMCSettings/Language`);
    }
    // 开启特定语言
    public openLang(params): Observable<any> {
        return this.http.post('/UI/Rest/BMCSettings/Language/OpenLanguage', params);
    }
    // 关闭特定语言
    public closeLang(params): Observable<any> {
        return this.http.post('/UI/Rest/BMCSettings/Language/CloseLanguage', params);
    }
}
