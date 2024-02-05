export interface IConditionsTitle {
    title: string;
    id: number;
    items: object[];
    selected: string;
}
export interface ITabs {
    title: string;
    id: string;
    active: boolean;
}
export interface IFilterSelect {
    text: string;
    id: string;
    show: boolean;
    uniq: string | string[];
}
