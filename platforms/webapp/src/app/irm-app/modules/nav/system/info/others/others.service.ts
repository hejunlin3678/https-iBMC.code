import { Injectable } from '@angular/core';
import { HttpService } from 'src/app/common-app/service';
import { NavTree } from './models';
import { map } from 'rxjs/operators';
import { CardUrlArray } from './models/cardUrlArray';

@Injectable({
    providedIn: 'root'
})
export class OthersService {

    public navTree: NavTree;
    public activeCard: CardUrlArray;

    constructor(
        private http: HttpService
    ) {
        this.navTree = new NavTree();
    }

    init() {
        return this.getBoards().pipe(
            map((postBody) => {
                this.parseChassisVal(postBody);
            })
        );
    }

    getBoards() {
        return this.getData('/UI/Rest/System/Boards');
    }

    parseChassisVal(bodyData) {
        const boards = bodyData;
        for (const board of Object.keys(boards)) {
            const cardUrl: string[] = [];
            cardUrl.push(boards[board].DetailLink);
            const cardUrlArray = new CardUrlArray(cardUrl, board, board, boards[board].MaxNumber, boards[board].CurrentNumber);
            this.navTree.addCardUrl(cardUrlArray);
        }
    }


    /**
     *  发送请求
     *  @param url 请求链接
     */
    public getData(url: string) {
        return this.http.get(url).pipe(
            map(
                (data: any) => data.body
            )
        );
    }
}
