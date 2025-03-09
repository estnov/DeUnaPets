import { Component } from '@angular/core';
import { Router } from '@angular/router';
import {
  trigger,
  state,
  style,
  transition,
  animate
} from '@angular/animations';

@Component({
  selector: 'app-first-loading',
  imports: [],
  templateUrl: './first-loading.component.html',
  styleUrl: './first-loading.component.scss',
  animations: [
    trigger('fadeAnimation', [
      state('in', style({ opacity: 1 })),
      state('out', style({ opacity: 0 })),

      transition('in => out', [
        animate('0.5s ease-out') 
      ]),
      transition('out => in', [
        animate('0.5s ease-in')
      ])
    ])
  ]
})
export class FirstLoadingComponent {
  fadeState: 'in' | 'out' = 'in';

  constructor(private router: Router) {}

  ngOnInit(): void {
    setTimeout(() => {
      this.router.navigate(['/create-pet']);
    }, 4000); 
  }
  
}
