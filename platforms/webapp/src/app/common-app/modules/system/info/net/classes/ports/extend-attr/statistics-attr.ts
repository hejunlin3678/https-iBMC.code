import { SendAttr } from './send-attr';
import { RecvAttr } from './recive-attr';

export class StatisticsAttr {
    public attr = [];
    constructor(
        send: SendAttr,
        recive: RecvAttr,
    ) {
        this.attr.push({
            title: 'SEND_STATISTICS',
            info: send
        }, {
            title: 'RECEIVE_STATISTICS',
            info: recive
        });
    }
}
