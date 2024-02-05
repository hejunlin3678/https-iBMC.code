import { Port } from '../port-attr';
import { TeamExtendAttr } from './team-extend-Attr';

export class TeamPort extends Port {
    netCardName: string;
    netName: string;
    port: string;
    macAddress: string;
    state: string;

    constructor(teamExtendAttr: TeamExtendAttr) {
        super(teamExtendAttr);
    }
}
