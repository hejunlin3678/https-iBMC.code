import { ROLES } from './data.types';

export interface IBmcUser {
    id?: number;
    userName?: string;
    role?: ROLES;
    belongTo?: string;
    url?: string;
}

export interface IBlade {
    list?: IBmcUser[];
    id?: string;
    label?: string;
}
