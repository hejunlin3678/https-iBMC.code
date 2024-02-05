import { Injectable } from '@angular/core';
import { HttpService } from 'src/app/common-app/service/httpService.service';
import { map } from 'rxjs/internal/operators/map';
import {IPartity} from '../hard-partity.component';

@Injectable({
    providedIn: 'root'
})
export class PartService {
    constructor(
        private http: HttpService,
    ) {}
    getSettings() {
        return this.http.get('/UI/Rest/System/FusionPartition').pipe(map((res) => {
            const response = res['body'];
            const hardPartity: IPartity = {
                fusionPartition : response.FusionPartition,
                vgaUSBDVDEnabled : response.VGAUSBDVDEnabled
            };
            return hardPartity;
        }));
    }
    setSetting(params) {
        return this.http.patch(
            `/UI/Rest/System/FusionPartition`,
            params
        );
    }
}
