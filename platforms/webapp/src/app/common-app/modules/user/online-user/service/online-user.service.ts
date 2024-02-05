import { Injectable } from '@angular/core';
import { HttpService } from 'src/app/common-app/service';

@Injectable({
    providedIn: 'root'
})
export class OnlineUserService {

    public findSessions() {
        return this.http.get('/UI/Rest/Sessions');
    }

    public deleteSessionById(id: string) {
        return this.http.delete(`/UI/Rest/Sessions/${id}`);
    }

    constructor(
        private http: HttpService
    ) { }

}
