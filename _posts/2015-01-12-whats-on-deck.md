---
published: true
category: blog
layout: blog
---

I'm guessing it's gonna be another light week at WSI, but I'll know for sure after tomorrow morning's standup. They shuffled some teams around and my standup is now at 11:00am instead of 9:45, which I'm not a super fan of. I also don't know how much need my team really has for my Java skillz; we went over the 2015 roadmap last week and the projects looked really front-end heavy. I guess I could see if those guys need any help, I'm trying to beef up my Javascript anyway to write the gabbo client.

There's also this project the director asked to do months ago that I've basically been ignoring. I made a mention in a meeting that it'd be nice if our reference docs were searchable, and he thought it was a good idea and asked me to do it. It's not that hard, it's just a bunch of markdown documents and a Solr implementation, which is this search solution by um Apache I think.

I was actually excited to do it when he first asked me but a couple things slowed me down on it. First, no one could tell me what project to book time to for it. That's probably because there isn't one, but that's a whole other topic. It's probably more an excuse than a reason, anyway. I think the real reason is because we're already using Solr for another internal tool and I feel a little constrained I guess, to try and follow their example. I'm not sure why exactly, no one asked me to even look at their stuff; but it's been vetted by our architect, who's easily in the top three programmers I've ever met, so why would I want to do it any other way? By comparison, working on gabbo is so liberating because I get to be the architect instead of trying to follow one. Even moreso since I started the new LDmud-based version, because I've got all this perspective built up over the years from EotL. This post isn't about gabbo, though.

The real project I wanted to write about is a sort of code review thing. I haven't really said anything yet, but I really want to try make the transition to working on gabbo exclusively by the end of 2015. It's a longshot goal, given I have absolutely no idea where the money would come from. Either way, I've kinda been working on an exit strategy from WSI, or at least Shop/Buy for a little while. I think my first priority has to be to leave the code in better shape than it was when I started. 

Now, I can't just go on a warpath fixing things unfit for my delicate sensibilities, because there's process and QA and all that. One thing I can do though, is document what needs to be done, and if I have the opportunity to knock some things off the list, even better. I think I want to do this with our code review tool, Crucible by Atlassian.

There's a bunch of stuff about Crucible that I like. I like that a review is basically a collection of commits, because there isn't really anything else inside the organization that's partitioned that way. We tend to set our reviews at the epic level, and it's really handy to be able to look at a review and see all the code that had to change for a feature. I also love that there's a comment engine. You can end up having whole design discussions right there in the source code, and a lot of times that's absolutely the best way to work through a problem. Plus it's all documented.

So what I'm thinking I wanna do is I set up a series of code reviews, on trunk, for different aspects of our application (the parts our team owns at least). Then just walk through the code a start leaving comments in the review. These aren't reviews you'd be trying to close; they'd be open ended and you just keep appending commits and adding comments. Crucible also lets you create Jira tickets for stuff out of comments too, I believe, if you decide you want to actually act on something. What'd be really cool is if you could get people's Crucible comments to show up in Eclipse. Then when you're working on something you have all these little pointers around whatever you're working on of other things you could do along the way. 

Damn it's gotten me thinking about gabbo again, and how I'd set up something like this there. Guess that means it's time to say good night.
