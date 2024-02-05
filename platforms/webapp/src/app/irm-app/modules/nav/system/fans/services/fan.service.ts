import { Injectable } from '@angular/core';
import { HttpService } from 'src/app/common-app/service';
@Injectable({
    providedIn: 'root'
})
export class FanService {
    constructor(
        private http: HttpService,
    ) { }

    public getThermals() {
        return this.http.get('/UI/Rest/System/Thermal');
    }
}
