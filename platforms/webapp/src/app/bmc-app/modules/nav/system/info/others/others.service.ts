import { Injectable } from '@angular/core';
import { GlobalDataService, HttpService, PRODUCTTYPE } from 'src/app/common-app/service';
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
        private http: HttpService,
        private globalData: GlobalDataService
    ) {
        this.navTree = new NavTree();
    }

    init() {
      return  this.getBoards().pipe(
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
        if (boards.hasOwnProperty('LeakageDetectionCard') && this.globalData.productType === PRODUCTTYPE.WUHAN ) {
            delete boards['LeakageDetectionCard'];
        }
        for (const board of Object.keys(boards).sort()) {
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

export function boardIdCUidShow(columns: any[], data: any[]) {
    // 是否有一条数据包含componentUniqueID
    const UID = data.find(item => item.componentUniqueID && item.componentUniqueID !== 'undefined');
    data.forEach(item => {
        // 如果componentUniqueID为'undefined'则该条数据无componentUniqueID
        // 如果数据中有一条包含componentUniqueID则需保留componentUniqueID，否则需删除componentUniqueID数据与表格数据匹配
        if (item.componentUniqueID === 'undefined') {
            if (UID) {
                item.componentUniqueID = '--';
            } else {
                delete item.componentUniqueID;
            }
        }
        // boardId： 为 0x00 或者 0xFFFF 显示为 --
        if (item.boardId === '0x00' || item.boardId === '0xFFFF') {
            item.boardId = '--';
        }
    });
    // 如果有componentUniqueID，添加componentUniqueID表头
    if (UID) {
        let componentUniqueIDIndex = 0;
        const componentUniqueIDHead = {
            title: 'OTHER_COMPONENT_UNIQUE_ID'
        };
        columns.forEach((item, index) => {
            if (item.title === 'OTHER_BOARD_ID') {
                componentUniqueIDIndex = index + 1;
            }
        });
        if (!columns.find(item => item.title === 'OTHER_COMPONENT_UNIQUE_ID')) {
            columns.splice(componentUniqueIDIndex, 0, componentUniqueIDHead);
        }
    }
}
