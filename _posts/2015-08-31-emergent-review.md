---
published: true
category: blog
layout: blog
---
I thought I'd post a conversation from earlier tonight on EotL. It started out with some harmless bitching about a wizard taking the review process a little too personally and led to a quick dialog about motivation and emergent behavior. This comes as I'm about to introduce a new package I've developed to integrate the mudlib with GitHub, but I have to sell to the wizards first. Having access to some richer tools I think will be embraced by most of the lower wizards, even if it does force them to adjust their workflow a bit. The higher ups will probably be a bit harder to convince since they can be resistant to embracing external tech, but I've put together a pretty good looking MUD-based interface to GitHub's tools that I'm hoping will sway them. There's another piece to the pitch that I haven't told anyone about yet that will be the real challenge: I'm gonna try to convince EotL to go open-source. I'm still working on that one. 

Anyway, here's the log I promised:<!--
--><code><pre>Guido says: seriously though someone needs to talk to watcher about his
            reactions to people taking the time to review his stuff, because
            it's not good
Taplash says: did he blow out on someone else?
Poi says: i'll nominate arphen, i'm not good with people
Taplash says: then after arphen talks to him, maybe arphen can talk to him
              about abuse of massive resist/stats/base damage.
Poi says: most of his armor has like +a bunch of something and -a bunch to
          other stuff, which isn't THAT big of a deal
Poi says: but some of it is just bonkers
Taplash says: some of it is within the realm of adjustment
You say: so make him adjust it and if he's got a problem with that we'll
         adjust it for him
You say: tho maybe let arphen take a stab at it first he's got more patience
         for that sorta thing than I do
Taplash says: i tried to review his area and got run off for saying it
              doesn't feel immersive.
You say: well when it comes to more subjective criteria like immersion I'm a
         little bit better equipped for that sorta thing
Taplash says: give it a go.
You say: but my general position is I'll review it and you either take my
         suggestions or you don't
Taplash says: yeh, just be really careful.
You say: in the old days I'd wear someone down until they gave up hope but
         these days I'll let someone put something online that I'm not into
         if I don't think it's gonna damage the game
Taplash says: because he gets really upset and goes off.
Poi says: i doubt he'd get like that with devo
Poi says: yeah you intereviewed me when i applied as a frob last time
Taplash says: i made it 12 hallways of repeated descs before he asked me
              what i thought.
Poi says: i've never been good at turning what in my head into words so i
          failed pretty miserably
Guido says: I would not recommend that devo do it
Taplash says: then it was ~1 hour of him having an emotional fit ending in
              him blocking me.
Taplash says: I think he'll listen to devo.
Guido says: as there is a personality mismatch there
You say: no guido is probably right
You say: he'd hear me out but I dunno that I'd actually prompt action
Taplash says: well you won't prompt action.
Taplash says: as he's pretty rigid.
Guido says: ok well time for operation: relocate cable modem
Poi says: every time i give him advice on something, he asks for my input on
          every little decision for the next week
Guido utters a small prayer for success.
Taplash says: if i try to give him advice he gets super emotional, freaks
              out, won't talk to me, then gives me a long apology later.
Taplash says: i was really careful this time, i picked the wording so as to
              not be confrontational, or tried.
You say: yeah he keeps apologizing to me in tells for shit I say to him on
         wiz channel
Taplash says: and was met with a lot of crazy.
Guido says: I think Poi likely has the best chance, followed by Arphen
Guido says: but neither may have the time/inclination
Guido says: such is life
Guido says: ok modemtime
Taplash says: he's really unstable, and i don't really like to get into it
              with him. but at the same time EoTL is a game created by a
              community and if you can't handle being part of the community
              something is wrong. it's not a free ride because x emotional
              issue.
You agree with Taplash wholeheartedly.
You say: toughen up or get out it's the EotL way
Taplash says: that's my feeling on it.
Poi says: he respects my sense of balance i think, and my coding ability,
          but i just can't sit down with him for hours and explain to him
          why item a is balanced, and item b is fucking ridiculous. these
          things should be apparent
Taplash says: but it's not get out, it's continue to make bad things and
              freak out at any percieved challenge to the
              area/code/whatever.
You say: well this is why I'm spending all this time trying to provide some
         decent peer review tools...it's a long road to actually making shit
         happen for real
You say: but I think formal tools for this sorta thing would help take the
         personal element out of it
You say: as much as you can at least
Taplash says: there's nothing personal in saying this extremely high damage
              high end weapon or armor that is limitless that you can wear
              at eval 1 is over powered.
You say: not to you or me
You say: but like
You say: take tells or says
You say: you use them here to often communicate very personal information
You say: and then you use the same channels to review code
You say: it's easy for someone on the receiving end to conflate the two
You say: with a separate device for doing code review, it makes it a little
         more distinct from everyday conversation
You say: which can be very personal
You say: I mentioned the wiz cahnnel/tell thing re: watcher and that's kind
         of an example of what I'm talking about
You say: I'm using wiz channel to talk about x and he responds in tells
You say: why? he's personalized it to some degree and moved it off channel
Taplash says: i think that you are correct in separating the review process
              from the having fun chatting process will help with reviews. I
              don't think it will stop someone from taking the reviews
              personally/bringing it in game.
You say: sure there's definitely a haters-gonna-hate element at work too
         that can't be avoided
You say: but you can at least try to mitigate it to some degree
Taplash says: i think having a peer review github thing will work, but I
              would also like to see some standards enforcement, or some
              sort of concensus on standards.
Taplash says: That way it isn't a 'well i coded this cause i dig everyone
              having 2000 str anyone who says otherwise just doesn't agree'
              instead there is a game design that we're targeting.
Guido says: I mean redwing and anthrax are not much better about taking
            criticisms on their code
Poi says: i personally take the porn approach to item balance, a'la "i know
          it when i see it"
You say: standards are important but ideally that's emergent phenomena and
         not dictated, it's more sustainable if nothing else
Guido says: I uhh don't agree that emergent phenomena standards are more
            sustainable
Guido says: at all
You say: i know you don't
Taplash says: what we have now is a lot of code designed specifically to
              boost the playerbase up to even higher evals.
Taplash says: which is what an RPG is, but we're off the deepend on power
              creep.
Taplash says: its not even power creep anymore, it's power sprint
Taplash laFs.
You say: when I say sustainable I'm talking about something where like...I
         trust arphen but in his approach to regulation and his judgement on
         balance et al
You say: so lets say I go 'yo arphen youre in charge make sure nothing
         stupid gets online'
You say: and im sure hed do a pretty good job
Taplash says: he would.
You say: but then what happens when he's done in kosovo, comes home gets a
         new job and can't mud anymore
Poi says: i think he leans a little too far on the conservative side, but
          yeah
Taplash says: it's also a really large task which needs to be divided.
Taplash says: because arphen is busy, as is everyone else who plays.
You say: emergent regulation doesn't have a single point of failure, or
         hopefully doesn't
Guido says: yeah, true, but it also can pretty easily end up with, say, what
            eotl's done over the past 10 years
You say: you're not wrong about that
Guido says: which is a case study on emergent regulatory behavior
Guido says: in MUDs anyhow
You say: I think we can keep with the spirit of the thing but do better
Poi says: by emergent regulation, you mean just let shit slide until it's
          brought to our attention?
You say: no that is not at all what I mean
Poi says: i've never heard the phrase before
You say: I mean creating a culture where bad ideas get buried and good ideas
         bubble to the top
Poi says: who's the man with the shovel?
You say: we all are
Taplash says: it's hard to make a culture when discussions about goals are
              off limits, or outright shouted down on either side.
You say: bear in mind I'm an unrepentant idealist
Taplash says: not the theoretical side that we're talking here.
Taplash says: the real side where people are actually making things.
Taplash says: in theory everyone wants to make a good game. though not
              everyone is looking into the future effects of the items they
              create. which leads wizards to doing damage control on these
              items, in the meantime other wizards are making those same
              items, only more powerful.
You say: not everyone wants to make a good game, some people just want to be
         loved even if it's at the expense of the game
Poi says: bingo
Taplash says: if that's the situation they shouldn't be coding as it leads
              to more issues later.
Poi says: redwing and watcher are both guilty of that                . ^R
You say: maybe. love's a powerful motivator it'd be a shame to waste it
Taplash says: it's really kind of a perverse situation though.
You say: you should be used to that by now
Taplash says: because the affection/appreciation is based on a playerbase
              manipulation.
Taplash grins at you.
Taplash nods nof nof nof nof.
You say: the whole game is based on manipulation
You say: people spend hours for what? flipping a few numbers and bits on a
         computer?
Guido says: that's different than the type of manipulation taplash is
            talking about
You say: the question is whether it's fun or not
You nod at Guido.
Taplash says: for the players or for the people seeking attention.
Taplash says: i know they're often one and the same.
Guido says: ok well as usual I have to go right when the conversation's
            steering towards something I've been trying to have for years
Guido says: it's funny I went through my old mudmail today as a way to avoid
            working on my dissertation
Taplash says: bring it up tomorrow.
Taplash says: cause i have to sleep.
Taplash says: and it's an interesting conversation.
Guido says: and I've spent the last 12 years fighting the same old fight one
            item/skill at a time
Guido says, with a sarcastic tone, "Wahoo."
Guido nods at Taplash.
Poi says: well before you go
Poi says: here's miley flashing her tity on mtv today
Poi says: http://i.imgur.com/2lziZ7J.gif
Taplash says: have you caught up yet guido? are you ready for the next
              round?!@#!
Guido says: caught up with what
Guido thanks Poi graciously.
Taplash says: skills/items
Taplash grins at Guido.
Guido says: oh, hah
Guido says: there are always more; it is always worse
Guido waves.
Taplash waves.
You say: we'll have the conversation eventually
You say: when it's time
Taplash says: before i sleep i just want to make a final statement, i push
              for standards because it's a unifying goal. i love that there
              are active coders who really want to put their ideas in.
Taplash says: and i'd like to see it towards a neat tidy finished project.
You say: also if you want a good study into emergent behavior, I got pretty
         obsessed with the reddit button
Taplash says: oh i've read about that.
You say: it's a great example of how all sorts of diverse and complex things
         can come out of some very simple rules
</pre></code>
